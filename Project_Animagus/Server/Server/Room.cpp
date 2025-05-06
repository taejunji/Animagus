#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "Buffers.h"
#include "Session.h"
#include "ServerPacketHandler.h"


// GRoom 의 각 요소들 초기생성
std::array<RoomRef, ROOM_COUNT> GRoom{};

Room::Room()
{
    InitializeGame();
}

bool Room::Enter(PlayerRef player)
{
    //std::lock_guard lock(m_mutex);
    if (m_players.find(player->playerID) != m_players.end())
        return false;
    
    m_players.insert(make_pair(player->playerID, player));
    player->room.store(shared_from_this());
    m_playerCount++;

    return true;
}

bool Room::Leave(uint16 playerID)
{
    //std::lock_guard lock(m_mutex);
    
    if (m_players.find(playerID) == m_players.end())
        return false;

    PlayerRef player = m_players[playerID];
    player->room.store(shared_from_this());

    m_players.erase(playerID);
    m_playerCount--;

    return true;
}

void Room::Broadcast(SendBufferRef sendBuffer, uint16 execptID)
{
    //std::lock_guard lock(m_mutex);
    for (auto& p : m_players)
    {
        PlayerRef player = p.second;
        if (player->playerID == execptID) continue;
        if (player->playerID == 0) continue;

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

    std::cout << "Player Count - " << m_playerCount << std::endl;

    bool isHost = false;
    int n_pid = 0;
    if (m_playerCount % 2 == 0)
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

bool Room::HandleStartGame(PlayerRef player)
{
    std::lock_guard lock(m_mutex);

    int n_pid = 0;

    if (m_players.count(player->playerID) == 0)
    {
        bool success = Enter(player);
        if (success == false)
            std::cout << "Enter Room Error" << std::endl;
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
        newPlayer.spawn_index = 0;
        //newPlayer.spawn_index = m_playerCount % 4;
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(newPlayer);
        if (auto session = player->ownerSession.lock())
            session->Send(sendBuffer);

        n_pid = player->playerID;

#ifndef _DUMMYTEST
        //std::cout << "Send Enter Game Packet to " << player->playerID << " , Spawn Index: " << newPlayer.spawn_index << std::endl;
#endif

        //std::cout << n_pid << std::endl;
    }

    // 신입 플레이어에게 기존 플레이어들 정보 전송 + AI 정보 전송
    {
        SC_SPAWN_PKT oldPlayer;

        for (auto& item : m_players)
        {
            if (n_pid == item.first) continue;  // 자기 자신의 정보는 이미 보냈음

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
        Broadcast(sendBuffer, player->playerID);
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
    std::cout << "Leave PlayerID: " << p_id << std::endl;
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
    if (m_players.find(playerId) == m_players.end())
        return false;

    //std::cout << "Handle Move" << std::endl;

    // 적용
    PlayerInfo info = pkt.player_info;
    PlayerRef& player = m_players[playerId];
    player->x = info.x; player->y = info.y; player->z = info.z;
    player->rotation = info.rotation;

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
    Broadcast(sendBuffer, playerId);

    return true;
}

bool Room::HandleSkillLocked(Protocol::CS_USING_SKILL_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    const uint16 playerId = pkt.player_id;
    if (m_players.contains(playerId) == false)
        return false;

#ifndef _DUMMYTEST
    std::cout << "Player" << playerId << " Used Skill " << static_cast<int>(pkt.s_type) << std::endl;
    //std::cout << " " << pkt.x << " " << pkt.y << " " << pkt.z << std::endl;
#endif

    // 뭐 더 붙일 정보가 있나?

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
    Broadcast(sendBuffer, playerId);

    return true;
}

bool Room::HandleEnterAIPlayer(Protocol::CS_AI_ENTER_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    uint16 ownerID = pkt.player_id;
    uint16 aiID = pkt.ai_id;
    if (m_aiPlayers.contains(aiID) == true) return false;

    std::cout << "AI Enter: " << aiID << std::endl;

    AIPlayerRef ai = std::make_shared<AIPlayer>(pkt.x, pkt.y, pkt.z, pkt.rotation);
    ai->aiID = aiID;
    ai->playerID = aiID;
    ai->type = pkt.p_type;

    m_aiPlayers.insert(make_pair(aiID, ai));
    ai->room.store(shared_from_this());

    m_playerCount++;

    // Host 를 제외한 클라이언트에서는 AI 를 일반 NetworkPlayer 로 인식
    SC_SPAWN_PKT newPlayer;
    newPlayer.x = ai->x;
    newPlayer.y = ai->y;
    newPlayer.z = ai->z;
    newPlayer.rotation = ai->rotation;
    newPlayer.player_id = ai->aiID;
    newPlayer.p_type = ai->type;

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(newPlayer);
    Broadcast(sendBuffer, ownerID);

    return true;
}

bool Room::HandleAIMoveLocked(Protocol::CS_AI_MOVE_PKT& pkt, const uint16 ownerID)
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
    Broadcast(sendBuffer, ownerID);

    return true;
}

bool Room::HandleAISkillLocked(Protocol::CS_AI_USING_SKILL_PKT& pkt, const uint16 ownerID)
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
    Broadcast(sendBuffer, ownerID);

    return true;
}

bool Room::HandleDamageLocked(Protocol::CS_DAMAGE_PKT& pkt, const uint16 ownerID)
{
    std::lock_guard lock(m_mutex);

    const uint16 player_id = pkt.player_id;
    if (m_players.contains(player_id) == false && m_aiPlayers.contains(player_id) == false) return false;

    PlayerRef player;
    if (player_id < 100) player = m_players[player_id];
    else player = m_aiPlayers[player_id];
    player->playerHP = pkt.hp;
    //player->isAlive = pkt.isAlive;

#ifndef _DUMMYTEST
    std::cout << "Player#" << player_id << " Got Damage - HP: " << pkt.hp << std::endl;
#endif

    SC_UPDATE_HP_PKT updateHpPkt;
    updateHpPkt.player_id = player_id;
    updateHpPkt.room_id = 0;            // TODO: 로비에서 여러 룸 중 선택
    updateHpPkt.hp = pkt.hp;
    updateHpPkt.isAlive = pkt.isAlive;

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(updateHpPkt);
    Broadcast(sendBuffer, ownerID);

    return true;
}

bool Room::HandleTimeOverLocked(Protocol::CS_TIME_OVER_PKT& pkt)
{
#ifndef _DUMMYTEST
    std::cout << "Room#" << m_roomID << " Time Over" << std::endl;
#endif

    std::lock_guard lock(m_mutex);

    std::vector<std::pair<int16, int16>> sortedPlayers; // (id, HP) 쌍으로 저장
    sortedPlayers.reserve(m_players.size() + m_aiPlayers.size());

    for (auto& kv : m_players)
        sortedPlayers.emplace_back(std::make_pair(kv.second->playerID, kv.second->playerHP));
    for (auto& kv : m_aiPlayers)
        sortedPlayers.emplace_back(std::make_pair(kv.second->playerID, kv.second->playerHP));

    std::sort(sortedPlayers.begin(), sortedPlayers.end(),
        [](const std::pair<int16, int16>& a, const std::pair<int16, int16>& b) {
            return a.second > b.second;
        });

#ifndef _DUMMYTEST
    for (auto& item : sortedPlayers) {
        std::cout << item.first << ":" << item.second << ", ";
    }
#endif

    if (m_roundCount++ < 3)
    {
        // 우승자 정보 + ???
        SC_GAME_INIT_PKT initGamePkt;

        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(initGamePkt);
        Broadcast(sendBuffer, 0);

        InitializeGame();
    }
    else
    {
        m_roundCount = 0;
    }

    return true;
}

void Room::InitializeGame()
{
    m_players.clear();
    m_aiPlayers.clear();
    m_playerCount = 0;

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

        for (int i = 0; i < 20; ++i)
        {
            item.spawn_index[i] = static_cast<char>(pool[i]);
            item.item_level[i] = static_cast<char>(rand() % 3);
            //item.item_level[i] = static_cast<char>(1);
        }
        item.item_count = 20;
        item.zone_index = 0;

        m_itemInfo[0] = item;
    }

    {   // Zone2
        SC_SPAWN_ITEM_PKT item;

        std::vector<int> pool;
        pool.resize(49); ZeroMemory(pool.data(), sizeof(int) * 49);
        std::iota(pool.begin(), pool.end(), 0);

        std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::shuffle(pool.begin(), pool.end(), rng);

        for (int i = 0; i < 10; ++i)
        {
            item.spawn_index[i] = static_cast<char>(pool[i]);
            item.item_level[i] = static_cast<char>(rand() % 3);
        }
        item.item_count = 10;
        item.zone_index = 1;

        m_itemInfo[1] = item;
    }

    {   // Zone3
        SC_SPAWN_ITEM_PKT item;

        for (int i = 0; i < 10; ++i)
        {
            item.spawn_index[i] = static_cast<char>(i);
            item.item_level[i] = static_cast<char>(rand() % 3);
        }
        
        item.item_count = 10;
        item.zone_index = 2;

        m_itemInfo[2] = item;
    }

}

