#include "pch.h"
#include "Player.h"
#include "AIPlayer.h"
#include "Room.h"
#include "Buffers.h"
#include "Session.h"
#include "ServerPacketHandler.h"


RoomRef GRoom = std::make_shared<Room>();

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
        newPlayer.x = player->x;
        newPlayer.y = player->y;
        newPlayer.z = player->z;
        newPlayer.rotation = player->rotation;
        newPlayer.host = (m_playerCount % 2) == 1;   // TODO: host 기준 만들기
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(newPlayer);
        if (auto session = player->ownerSession.lock())
            session->Send(sendBuffer);

        n_pid = player->playerID;
        std::cout << "Send Enter Game Packet to " << player->playerID << std::endl;

        if (newPlayer.host == true)
        {
            m_hostPlayer = player;
            isHost = true;

            std::cout << n_pid << " is Host" << std::endl;
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

            std::cout << item.first << "'s info Send Spawn Packet to " << n_pid << std::endl;
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

    return success;
}

bool Room::HandleLeavePlayer(PlayerRef player)
{
    std::lock_guard lock(m_mutex);

    uint16 p_id = player->playerID;
    bool success = Leave(p_id);

    std::cout << "Leave PlayerID: " << p_id << std::endl;

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

    std::cout << "Player" << playerId << " Used Skill " << static_cast<int>(pkt.s_type) << std::endl;
    //std::cout << " " << pkt.x << " " << pkt.y << " " << pkt.z << std::endl;

    // 뭐 더 붙일 정보가 있나?

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
    Broadcast(sendBuffer, playerId);

    return true;
}

bool Room::HandleEnterAIPlayer(Protocol::CS_AI_ENTER_PKT& pkt)
{
    std::lock_guard lock(m_mutex);

    std::cout << "AI Enter" << std::endl;

    uint16 aiID = pkt.ai_id;
    if (m_aiPlayers.contains(aiID) == true) return false;

    AIPlayerRef ai = std::make_shared<AIPlayer>(pkt.x, pkt.y, pkt.z, pkt.rotation);
    ai->aiID = aiID;
    ai->type = pkt.p_type;

    m_aiPlayers.insert(make_pair(ai->aiID, ai));
    ai->room.store(shared_from_this());

    return true;
}

bool Room::HandleAIMoveLocked(Protocol::CS_AI_MOVE_PKT& pkt, const uint16 ownerID)
{
    std::lock_guard lock(m_mutex);
    const uint16 aiID = pkt.player_info.player_id;
    if (m_aiPlayers.contains(aiID) == false) return false;

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

