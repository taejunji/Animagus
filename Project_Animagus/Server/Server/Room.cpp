#include "pch.h"
#include "Player.h"
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
    }
}

bool Room::HandleEnterPlayer(PlayerRef player)
{
    std::lock_guard lock(m_mutex);

    bool success = Enter(player);
    if (success == false)
        std::cout << "Error" << std::endl;

    // 신입 플레이어 스폰 위치, 회전각 서버에서 지정해주고 해당 정보 플레이어에게 전송
    {
        SC_ENTER_GAME_PKT newPlayer;
        newPlayer.player_id = player->playerID;
        newPlayer.x = player->x;
        newPlayer.y = player->y;
        newPlayer.z = player->z;
        newPlayer.rotation = player->rotation;
        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(newPlayer);
        if (auto session = player->ownerSession.lock())
            session->Send(sendBuffer);
    }

    // 기존 플레이어들에게 신입 플레이어 정보 전송
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

    // 신입 플레이어에게 기존 플레이어들 정보 전송
    {
        SC_SPAWN_PKT oldPlayer;

        for (auto& item : m_players)
        {
            PlayerRef o_player = item.second;
            oldPlayer.x = o_player->x;
            oldPlayer.y = o_player->y;
            oldPlayer.z = o_player->z;
            oldPlayer.rotation = player->rotation;
            oldPlayer.player_id = player->playerID;
            oldPlayer.p_type = player->type;
            SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(oldPlayer);
            if (auto session = player->ownerSession.lock())
                session->Send(sendBuffer);
        }
    }

    return success;
}

bool Room::HandleLeavePlayer(PlayerRef player)
{
    std::lock_guard lock(m_mutex);

    uint16 p_id = player->playerID;
    bool success = Leave(p_id);

    // 다른 플레이어에게 해당 플레이어 퇴장 알림
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

    const uint64 playerId = pkt.player_info.player_id;
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
