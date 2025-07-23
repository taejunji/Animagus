#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "Buffers.h"
#include "Session.h"
#include "ServerPacketHandler.h"

static std::random_device rd;
static std::mt19937       gen(rd());

// GRoom 의 각 요소들 초기생성
std::array<RoomRef, ROOM_COUNT> GRoom{};
std::array<uint8, 8> scoreBoard = {10, 7, 6, 5, 4, 3, 2, 1};

std::string AINameList[] = {
    "HwanHee", "TaeJun", "GwangSin", "JaeGyeong",
    "DaeHyeon", "YongSik", "JiWoong", "NaeHoon"
};

Room::Room()
{
    InitializeGame();
    InitAiTypes();
}

bool Room::Enter(PlayerRef player)
{
    //std::lock_guard lock(m_mutex);
    if (m_players.count(player->playerID) != 0)
        return false;
    
    m_players.insert(make_pair(player->playerID, player));
    player->s_mutex.lock();
    player->room.store(shared_from_this());
    player->player_state = PlayerRoomState::WAITING;
    player->s_mutex.unlock();
    m_playerNames[player->playerID] = player->name;
    m_playerCount++;

    return true;
}

bool Room::Leave(uint16 playerID)
{
    //std::lock_guard lock(m_mutex);
    
    if (m_players.count(playerID) == 0)
        return false;

    PlayerRef player = m_players[playerID];
    player->s_mutex.lock();
    player->room.store(shared_from_this());
    player->player_state = PlayerRoomState::LOBBY;
    player->s_mutex.unlock();
    m_players.erase(playerID);
    if (0 != m_playerNames.count(playerID))
        m_playerNames.erase(playerID);
    m_playerCount--;
    m_alivePlayerCount--;

    // TODO: Leave Pkt 전송

    if (player->playerID == m_hostPlayer->playerID) 
    {
        for (auto& ai : m_aiPlayers) {
            // TODO: Leave Pkt 전송

            if (0 != m_playerNames.count(ai.first))
                m_playerNames.erase(ai.first);
        }
        m_aiPlayers.clear();
    }

    if (m_playerCount == 0) {
        InitializeRoom();
    }

    return true;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint16 execptID, bool state_check)
{
    //std::lock_guard lock(m_mutex);
    for (auto& p : m_players)
    {
        PlayerRef player = p.second;
        if (player->playerID == execptID) continue;
        if (player->playerID == 0) continue;
        if (state_check == true) {
            if (player->player_state != PlayerRoomState::INGAME) continue;
        }

        SessionRef session = player->ownerSession.lock();
        session->Send(sendBuffer);

        //std::cout << "Send Packet to " << player->playerID << std::endl;
    }
}

bool Room::HandleEnterPlayer(PlayerRef player)
{
    std::lock_guard lock(m_mutex);

    bool success = Enter(player);
    if (success == false)
        std::cout << "Enter Room Error" << std::endl;


#ifndef _DUMMYTEST
    std::cout << "Room#" << m_roomID << " Player Enter :" << player->playerID << std::endl;
#endif

    std::cout << "Room#" << m_roomID  << " Player Count - " << m_playerCount << std::endl;

    bool isHost = false;
    int n_pid = 0;
    if (m_playerCount == 1)
    {
        SC_UR_HOST_PKT ur_host;
        m_hostPlayer = player;
        isHost = true;
        n_pid = player->playerID;

#ifndef _DUMMYTEST
        std::cout << n_pid << " is Host" << std::endl;
#endif

        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(ur_host);
        if (auto session = player->ownerSession.lock())
            session->Send(sendBuffer);
    }

    return success;
}

bool Room::HandleEnterGame()
{
    m_loadingOverCount++;
    if (m_loadingOverCount == m_players.size())
    {
        m_gameStartTickCount = GetTickCount64();
        for (auto& item : m_players) 
        {
            HandleStartGame(item.second);
            if (item.first == m_hostPlayer->playerID) {
                //MakeAIPlayer();
            }
        }
    }

    return true;
}

bool Room::HandleStartGame(PlayerRef player)
{
    std::lock_guard lock(m_mutex);

    int n_pid = 0;

    if (m_players.count(player->playerID) == 0)
    {
        std::cout << "Player didn't exist this Room" << std::endl;
        return false;
    }

    // 신입 플레이어 스폰 위치, 회전각 서버에서 지정해주고 해당 정보 플레이어에게 전송
    {
        SC_ENTER_GAME_PKT newPlayer;
        newPlayer.player_id = player->playerID;
        //newPlayer.x = player->x;
        //newPlayer.y = player->y;
        //newPlayer.z = player->z;
        //newPlayer.rotation = player->rotation;
        newPlayer.host = (player->playerID == m_hostPlayer->playerID);
        //newPlayer.spawn_index = 0;
        newPlayer.spawn_index = m_indexGen++ % m_maxPlayerCount;
        newPlayer.server_time = m_gameStartTickCount;
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(newPlayer);
        if (auto session = player->ownerSession.lock())
            session->Send(sendBuffer);

        n_pid = player->playerID;

        player->s_mutex.lock();
        player->player_state = PlayerRoomState::INGAME;
        player->s_mutex.unlock();

#ifndef _DUMMYTEST
        //std::cout << "Send Enter Game Packet to " << player->playerID << " , Spawn Index: " << newPlayer.spawn_index << std::endl;
#endif

        //std::cout << n_pid << std::endl;
    }

    // 호스트라면 AI 스폰 정보 전송
    {
        if (player->playerID == m_hostPlayer->playerID) 
        {
            SC_AI_SPAWN_PKT aiSpawn;
            aiSpawn.player_count = static_cast<int16>(m_players.size());
            for (int i = 0; i < m_maxPlayerCount; ++i) {
                aiSpawn.types[i] = aiPlayerTypes[i];
                //std::cout << static_cast<int>(aiSpawn.types[i]) << std::endl;
            }

            SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(aiSpawn);
            if (auto session = player->ownerSession.lock())
                session->Send(sendBuffer);

            std::cout << "AI Spawn" << std::endl;
        }
    }

    // 신입 플레이어에게 기존 플레이어들 정보 전송 + AI 정보 전송
    {
        SC_SPAWN_PKT oldPlayer;

        for (auto& item : m_players)
        {
            if (n_pid == item.first) continue;  // 자기 자신의 정보는 이미 보냈음
            if (n_pid == 0) continue;           // 아이디가 0인 플레이어를 마주쳤다면 도망치십시오
            if (item.second->player_state != PlayerRoomState::INGAME) continue;

            PlayerRef o_player = item.second;
            oldPlayer.x = o_player->x;
            oldPlayer.y = o_player->y;
            oldPlayer.z = o_player->z;
            oldPlayer.rotation = o_player->rotation;
            oldPlayer.player_id = o_player->playerID;
            oldPlayer.p_type = o_player->type;

            SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(oldPlayer);
            if (auto session = player->ownerSession.lock())
                session->Send(sendBuffer);

#ifndef _DUMMYTEST
            //std::cout << item.first << "'s info Send Spawn Packet to " << n_pid << std::endl;
#endif
        }

        for (auto& item : m_aiPlayers)
        {
            if (m_hostPlayer->playerID == player->playerID) break;  // host 에게 ai 정보는 필요 없음

            AIPlayerRef ai_player = item.second;
            oldPlayer.x = ai_player->x;
            oldPlayer.y = ai_player->y;
            oldPlayer.z = ai_player->z;
            oldPlayer.rotation = ai_player->rotation;
            oldPlayer.player_id = ai_player->aiID;
            oldPlayer.p_type = ai_player->type;

            SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(oldPlayer);
            if (auto session = player->ownerSession.lock())
                session->Send(sendBuffer);

            //std::cout << "AI info Send Spawn Packet to " << n_pid << std::endl;
        }

    }

    // 기존 플레이어들에게 신입 플레이어 정보 전송
    // 현재 Host 조건이 해당 Room 의 홀수번째 플레이어이기 때문에 기존 플레이어는 이미 AI 정보 갖고있음
    {
        SC_SPAWN_PKT newPlayer;
        newPlayer.x = player->x;
        newPlayer.y = player->y;
        newPlayer.z = player->z;
        newPlayer.rotation = player->rotation;
        newPlayer.player_id = player->playerID;
        newPlayer.p_type = player->type;
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(newPlayer);
        Broadcast(sendBuffer, player->playerID, true);
    }

    // 신입 플레이어에게 아이템 정보 전송
    for (uint16 i = 0; i < 3; ++i)
    {
        SC_SPAWN_ITEM_PKT item = m_itemInfo[i];

        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(item);
        if (auto session = player->ownerSession.lock())
            session->Send(sendBuffer);
    }


    return true;
}

bool Room::HandleLeavePlayer(PlayerRef player)
{
    std::lock_guard lock(m_mutex);

    uint16 p_id = player->playerID;
    bool success = Leave(p_id);

#ifndef _DUMMYTEST
    std::cout << "Room#" << m_roomID << " Leave PlayerID: " << p_id << std::endl;
#endif

    // 다른 플레이어에게 해당 플레이어 퇴장 알림 + Host 라면 AI 플레이어 퇴장 일림
    {
        //SC_LEAVE_PKT pkt;
        //pkt.player_id = player->playerID;
        //SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
        //Broadcast(sendBuffer, 0);
    }

    m_aiPlayers.clear();

    return success;
}

bool Room::HandleMoveLocked(Protocol::CS_MOVE_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    const uint16 playerId = pkt.player_info.player_id;
    //std::cout << "Move PlayerID: " << playerId << std::endl;
    if (m_players.count(playerId) == 0)
        return false;

    //std::cout << "Handle Move" << std::endl;

    // 적용
    PlayerInfo info = pkt.player_info;
    PlayerRef& player = m_players[playerId];
    player->x = info.x; player->y = info.y; player->z = info.z;
    player->rotation = info.rotation;

    pkt.server_time = GetTickCount64();

    //std::cout << info.x << " " << info.y << " " << info.z << std::endl;
    //std::cout << pkt.player_info.speed << std::endl;

    //if (pkt.player_info.player_id == 1) {
    //    if (pkt.player_info.player_state == Protocol::PlayerState::MOVE_STATE_JUMP)
    //        std::cout << "Jump" << std::endl;
    //    if (pkt.player_info.player_state == Protocol::PlayerState::MOVE_STATE_RUN)
    //        std::cout << "RUN" << std::endl;
    //    if (pkt.player_info.player_state == Protocol::PlayerState::MOVE_STATE_IDLE)
    //        std::cout << "IDLE" << std::endl;
    //}

    // 이동 
    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
    Broadcast(sendBuffer, 0, true);

    return true;
}

bool Room::HandleSkillLocked(Protocol::CS_USING_SKILL_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    const uint16 playerId = pkt.player_id;
    if (m_players.contains(playerId) == false)
        return false;

#ifndef _DUMMYTEST
    std::cout << "Room#" << m_roomID << " Player" << playerId << " Used Skill " << static_cast<int>(pkt.s_type) << std::endl;
    //std::cout << " " << pkt.x << " " << pkt.y << " " << pkt.z << std::endl;
#endif

    // 뭐 더 붙일 정보가 있나?

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
    Broadcast(sendBuffer, playerId, true);

    return true;
}

bool Room::HandleEnterAIPlayer(const Protocol::CS_AI_ENTER_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    uint16 ownerID = pkt.player_id;
    uint16 aiID = pkt.ai_id;
    if (m_aiPlayers.contains(aiID) == true) return false;

    std::cout << "Room#" << m_roomID << "AI Enter: " << aiID << ", Owner: " << ownerID << ", Type: " << static_cast<int>(pkt.p_type) << std::endl;

    AIPlayerRef ai = std::make_shared<AIPlayer>(pkt.x, pkt.y, pkt.z, pkt.rotation);
    ai->aiID = aiID;
    ai->playerID = aiID;
    ai->type = pkt.p_type;

    m_aiPlayers.insert(make_pair(aiID, ai));
    ai->room.store(shared_from_this());

    if (false == m_playerNames.contains(aiID))
    {
        m_aiNameGen = m_aiNameGen >= m_maxPlayerCount - 1 ? 0 : m_aiNameGen + 1;
        m_playerNames[aiID] = AINameList[m_aiNameGen];
    }

    //m_playerCount++;

    // Host 를 제외한 클라이언트에서는 AI 를 일반 NetworkPlayer 로 인식
    SC_SPAWN_PKT newPlayer;
    newPlayer.x = ai->x;
    newPlayer.y = ai->y;
    newPlayer.z = ai->z;
    newPlayer.rotation = ai->rotation;
    newPlayer.player_id = ai->aiID;
    newPlayer.p_type = ai->type;

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(newPlayer);
    Broadcast(sendBuffer, ownerID, true);

    return true;
}

bool Room::HandleAIMoveLocked(const Protocol::CS_AI_MOVE_PKT& pkt, const uint16 ownerID)
{
    std::lock_guard lock(m_mutex);
    const uint16 aiID = pkt.player_info.player_id;
    if (m_aiPlayers.contains(aiID) == false) return false;

    //std::cout << "AI Move" << std::endl;

    PlayerInfo info = pkt.player_info;
    AIPlayerRef& player = m_aiPlayers[aiID];
    player->x = info.x; player->y = info.y; player->z = info.z;
    player->rotation = info.rotation;

    Protocol::CS_MOVE_PKT movePkt;
    movePkt.player_info.x = player->x;
    movePkt.player_info.y = player->y;
    movePkt.player_info.z = player->z;
    movePkt.player_info.rotation = player->rotation;
    movePkt.player_info.player_id = player->aiID;
    movePkt.player_info.player_state = pkt.player_info.player_state;
    movePkt.player_info.speed_2d = pkt.player_info.speed_2d;
    movePkt.player_info.speed_z = pkt.player_info.speed_z;

    //if (movePkt.player_info.player_id == 101)
    //{
    //    if (pkt.player_info.player_state == Protocol::PlayerState::MOVE_STATE_JUMP)
    //        std::cout << "Jump" << std::endl;
    //    if (pkt.player_info.player_state == Protocol::PlayerState::MOVE_STATE_RUN)
    //        std::cout << "RUN" << std::endl;
    //    if (pkt.player_info.player_state == Protocol::PlayerState::MOVE_STATE_IDLE)
    //        std::cout << "IDLE" << std::endl;
    //}

    // host 에게는 전송 X
    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
    Broadcast(sendBuffer, ownerID, true);

    return true;
}

bool Room::HandleAISkillLocked(const Protocol::CS_AI_USING_SKILL_PKT& pkt, const uint16 ownerID)
{
    std::lock_guard lock(m_mutex);

    const uint16 aiID = pkt.ai_id;
    if (m_aiPlayers.contains(aiID) == false) return false;

    CS_USING_SKILL_PKT aiSkillPkt;
    aiSkillPkt.player_id = aiID;
    aiSkillPkt.room_id = pkt.room_id;
    aiSkillPkt.pitch = pkt.pitch;
    aiSkillPkt.yaw = pkt.yaw;
    aiSkillPkt.roll = pkt.roll;
    aiSkillPkt.s_type = pkt.s_type;

    //std::cout << "AI Using Skill " << static_cast<int>(aiSkillPkt.s_type) << " Rotation: " << aiSkillPkt.pitch << ", " << aiSkillPkt.yaw << ", " << aiSkillPkt.roll << std::endl;

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(aiSkillPkt);
    Broadcast(sendBuffer, ownerID, true);

    return true;
}

bool Room::HandleDamageLocked(const Protocol::CS_DAMAGE_PKT& pkt, const uint16 ownerID)
{
    const uint16 player_id = pkt.player_id;
    std::lock_guard lock(m_mutex);

    if (m_players.contains(player_id) == false && m_aiPlayers.contains(player_id) == false) return false;

    PlayerRef player;
    if (player_id < 100) player = m_players[player_id];
    else player = m_aiPlayers[player_id];
    player->playerHP = pkt.hp;
    //player->isAlive = pkt.hp > 0;

#ifndef _DUMMYTEST
    //std::cout << "Room#" << m_roomID << " Player#" << player_id << " Got Damage - HP: " << pkt.hp << std::endl;
#endif

    SC_UPDATE_HP_PKT updateHpPkt;
    updateHpPkt.player_id = player_id;
    //updateHpPkt.room_id = 0;
    updateHpPkt.hp = pkt.hp;
    //updateHpPkt.isAlive = pkt.isAlive;

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(updateHpPkt);
    Broadcast(sendBuffer, ownerID, true);

    if (pkt.hp <= 0 && player->isAlive == true)
    {
        player->isAlive = false;
        m_alivePlayerCount--;
        std::cout << "Room#" << m_roomID << " Alive Player Count - " << m_alivePlayerCount << std::endl;
        if (m_alivePlayerCount == 1)
        {
            std::cout << "Room#" << m_roomID << " Last Player Standing" << std::endl;
            HandleRoundEndLocked(Protocol::CS_ROUND_END_PKT{});
        }
    }

    return true;
}

bool Room::HandleRoundEndLocked(const Protocol::CS_ROUND_END_PKT& pkt)
{
#ifndef _DUMMYTEST
    std::cout << "Room#" << m_roomID << " Time Over" << std::endl;
#endif

    //std::lock_guard lock(m_mutex);    // recursive locking error

    std::vector<std::pair<int16, int16>> sortedPlayersByHp; // (id, HP) 쌍으로 저장
    sortedPlayersByHp.reserve(m_players.size() + m_aiPlayers.size());

    for (auto& kv : m_players)
        sortedPlayersByHp.emplace_back(std::make_pair(kv.second->playerID, kv.second->playerHP));
    for (auto& kv : m_aiPlayers)
        sortedPlayersByHp.emplace_back(std::make_pair(kv.second->playerID, kv.second->playerHP));

    std::sort(sortedPlayersByHp.begin(), sortedPlayersByHp.end(),
        [](const std::pair<int16, int16>& a, const std::pair<int16, int16>& b) {
            return a.second > b.second;
        });

    uint8 scoreBoardIndex = 0;
    for (auto& item : sortedPlayersByHp) {
#ifndef _DUMMYTEST
        //std::cout << item.first << ":" << item.second << ", ";
#endif
        accumRanking[item.first] += scoreBoard[scoreBoardIndex++];
        std::cout << item.first << ":" << accumRanking[item.first] << ", ";
    }
    std::cout << std::endl;

    // 라운드 초기화 작업
    InitializeGame();

    SC_ROUND_END_PKT roundEndPkt;

    // 우승자 정보 + ???
    std::vector<std::pair<int16/*id*/, int16/*score*/>> sortedPlayersByScore;
    sortedPlayersByScore.reserve(m_maxPlayerCount);
    for (auto& p : accumRanking)
        sortedPlayersByScore.emplace_back(p);

    std::sort(sortedPlayersByScore.begin(), sortedPlayersByScore.end(),
        [](const std::pair<int16, int16>& a, const std::pair<int16, int16>& b) {
            return a.second > b.second;
        });

    for (int8 i = 0; i < 8; ++i) {
        roundEndPkt.ranking[i] = sortedPlayersByScore[i].first;
        strcpy_s(roundEndPkt.name[i], m_playerNames[sortedPlayersByScore[i].first].c_str());
        roundEndPkt.score[i] = sortedPlayersByScore[i].second;
    }

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(roundEndPkt);
    Broadcast(sendBuffer, 0);

    ++m_roundCount;

    return true;
}

bool Room::HandleRoundInitLocked(const Protocol::CS_ROUND_INIT_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    if (m_roundCount <= 3) {
        SC_ROUND_INIT_PKT roundInitPkt;
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(roundInitPkt);
        Broadcast(sendBuffer, 0);
    }
    else {
        // 로비로 보내기
        SC_GAME_END_PKT gameEndPkt;
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(gameEndPkt);
        Broadcast(sendBuffer, 0);

        InitializeRoom();
    }

    return true;
}

bool Room::HandleHitChangeSkill(const Protocol::CS_SKILL_CHANGE_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    uint16 hit_player_id = pkt.hit_player_id;
    uint16 shooter_player_id = pkt.shooter_player_id;

    float h_x = m_players[hit_player_id]->x, h_y = m_players[hit_player_id]->y, h_z = m_players[hit_player_id]->z;
    float s_x = m_players[shooter_player_id]->x, s_y = m_players[shooter_player_id]->y, s_z = m_players[shooter_player_id]->z;

    m_players[hit_player_id]->x = s_x; m_players[hit_player_id]->y = s_y; m_players[hit_player_id]->z = s_z;
    m_players[shooter_player_id]->x = h_x; m_players[shooter_player_id]->y = h_y; m_players[shooter_player_id]->z = h_z;



    return true;
}

bool Room::HandleJumpEffect(Protocol::CS_JUMP_EFT_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
    Broadcast(sendBuffer, pkt.jump_player_id, true);

    return true;
}

void Room::InitializeGame()
{
    //m_players.clear();
    for (auto& item : m_players)
    {
        PlayerRef player = item.second;
        player->s_mutex.lock();
        player->player_state = PlayerRoomState::WAITING;
        player->s_mutex.unlock();
    }
    m_aiPlayers.clear();
    //m_playerCount = 0;
    m_gameStartTickCount = 0;
    m_loadingOverCount = 0;
    m_indexGen = 0;
    m_alivePlayerCount = 8;

    InitItemInfo();
}

void Room::InitItemInfo()
{
    {   // Zone1
        SC_SPAWN_ITEM_PKT item; 

        std::vector<int> pool;
        pool.resize(90); ZeroMemory(pool.data(), sizeof(int) * 90);
        std::iota(pool.begin(), pool.end(), 0);

        std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::shuffle(pool.begin(), pool.end(), rng);

        for (int i = 0; i < 30; ++i)
        {
            item.spawn_index[i] = static_cast<char>(pool[i]);
            //int item_level = rand() % 10;
            //if (item_level <= 6)
            //    item.item_level[i] = static_cast<char>(0);
            //else if (item_level <= 8)
            //    item.item_level[i] = static_cast<char>(1);
            //else
            //    item.item_level[i] = static_cast<char>(2);
            item.item_level[i] = static_cast<char>(rand() % 3);
        }
        item.item_count = 30;
        item.zone_index = 0;

        m_itemInfo[0] = item;
    }

    {   // Zone2
        SC_SPAWN_ITEM_PKT item;

        std::vector<int> pool;
        pool.resize(45); ZeroMemory(pool.data(), sizeof(int) * 45);
        std::iota(pool.begin(), pool.end(), 0);

        std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::shuffle(pool.begin(), pool.end(), rng);

        for (int i = 0; i < 20; ++i)
        {
            item.spawn_index[i] = static_cast<char>(pool[i]);
            //int item_level = rand() % 10;
            //if (item_level <= 5)
            //    item.item_level[i] = static_cast<char>(0);
            //else if (item_level <= 8)
            //    item.item_level[i] = static_cast<char>(1);
            //else
            //    item.item_level[i] = static_cast<char>(2);
            item.item_level[i] = static_cast<char>(rand() % 3);
        }
        item.item_count = 20;
        item.zone_index = 1;

        m_itemInfo[1] = item;
    }

    {   // Zone3
        SC_SPAWN_ITEM_PKT item;

        for (int i = 0; i < 12; ++i)
        {
            item.spawn_index[i] = static_cast<char>(i);
            //int item_level = rand() % 10;
            //if (item_level <= 7)
            //    item.item_level[i] = static_cast<char>(0);
            //else
            //    item.item_level[i] = static_cast<char>(1);
            item.item_level[i] = static_cast<char>(rand() % 3);
        }
        
        item.item_count = 12;
        item.zone_index = 2;

        m_itemInfo[2] = item;
    }

}

void Room::InitAiTypes()
{
    std::vector<PlayerType> pool;
    pool.reserve(static_cast<size_t>(PlayerType::COUNT) - 1);
    for (uint16_t i = static_cast<uint16_t>(PlayerType::NONE) + 1;
        i < static_cast<uint16_t>(PlayerType::COUNT); ++i) {
        pool.push_back(static_cast<PlayerType>(i));
    }

    std::shuffle(pool.begin(), pool.end(), gen);

    std::copy_n(pool.begin(), aiPlayerTypes.size(), aiPlayerTypes.begin());
}

void Room::InitializeRoom()
{
    InitializeGame();
    InitAiTypes();

    m_players.clear();
    m_aiPlayers.clear();
    m_playerCount = 0;
    m_alivePlayerCount = 8;
    m_hostPlayer = nullptr;
    m_roundCount = 1;
    m_indexGen = 0;
    m_loadingOverCount = 0;
    m_gameStartTickCount = 0;
    m_playerNames.clear();
    m_aiNameGen = 0;
    accumRanking.clear();
    m_nowPlayerCount = 0;

}

