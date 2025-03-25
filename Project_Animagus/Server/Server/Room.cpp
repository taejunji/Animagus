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
    player->room.store(std::weak_ptr<Room>());

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

    // TODO : 신입 플레이어 스폰 위치, 회전각 서버에서 지정해주고 해당 정보 플레이어에게 전송
    {

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
