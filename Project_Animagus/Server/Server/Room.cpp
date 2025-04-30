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
        std::cout << "Error" << std::endl;

    int n_pid = 0;
    bool isHost = false;
    // 신입 플레이어 스폰 위치, 회전각 서버에서 지정해주고 해당 정보 플레이어에게 전송
    {
        SC_ENTER_GAME_PKT newPlayer;
        newPlayer.player_id = player->playerID;
        //newPlayer.x = player->x;
        //newPlayer.y = player->y;
        //newPlayer.z = player->z;
        //newPlayer.rotation = player->rotation;
        newPlayer.spawn_index = m_playerCount % 4;
        newPlayer.host = (m_playerCount % 2) == 1;   // TODO: host 기준 만들기
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(newPlayer);
        if (auto session = player->ownerSession.lock())
            session->Send(sendBuffer);

        n_pid = player->playerID;

#ifndef _DUMMYTEST
        std::cout << "Send Enter Game Packet to " << player->playerID << " , Spawn Index: " << newPlayer.spawn_index << std::endl;
#endif

        if (newPlayer.host == true)
        {
            m_hostPlayer = player;
            isHost = true;

#ifndef _DUMMYTEST
            std::cout << n_pid << " is Host" << std::endl;
#endif
        }
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
            std::cout << item.first << "'s info Send Spawn Packet to " << n_pid << std::endl;
#endif
        }

        for (auto& item : m_aiPlayers)
        {
            if ((m_playerCount % 2) == 1) break;  // host 에게 ai 정보는 필요 없음

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

            std::cout << "AI info Send Spawn Packet to " << n_pid << std::endl;
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
    for (uint16 i = 0; i < 2; ++i)
    {
        SC_SPAWN_ITEM_PKT item = m_itemInfo[i];

        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(item);
        if (auto session = player->ownerSession.lock())
            session->Send(sendBuffer);
    }

    return success;
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

    //if (pkt.player_info.player_state == Protocol::PlayerState::MOVE_STATE_JUMP)
    //    std::cout << "Jump" << std::endl;
    
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
    ai->type = pkt.p_type;

    m_aiPlayers.insert(make_pair(ai->aiID, ai));
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

void Room::InitializeGame()
{
    // 이 함수는 항상 스레드 하나에서만 호출함. 프로그램 시작 시, host 로부터 전투단계 종료 확인 시

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
            item.item_level[i] = static_cast<char>(rand() % 2);
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
            item.item_level[i] = static_cast<char>(rand() % 2);
        }
        item.item_count = 10;
        item.zone_index = 1;

        m_itemInfo[1] = item;
    }

    //{   // Zone3
    //    SC_SPAWN_ITEM_PKT item;

    //    std::vector<int> pool;
    //    pool.resize(49); ZeroMemory(pool.data(), sizeof(int) * 49);
    //    std::iota(pool.begin(), pool.end(), 0);

    //    std::mt19937_64 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    //    std::shuffle(pool.begin(), pool.end(), rng);

    //    for (int i = 0; i < 10; ++i)
    //    {
    //        item.spawn_index[i] = static_cast<char>(pool[i]);
    //        item.item_level[i] = static_cast<char>(rand() % 2);
    //    }
    //    item.item_count = 10;
    //    item.zone_index = 2;

    //    m_itemInfo[2] = item;
    //}

}

