#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "Buffers.h"

void Room::Enter(PlayerRef player)
{
    std::lock_guard lock(m_mutex);
    m_players[player->playerID] = player;
}

void Room::Leave(PlayerRef player)
{
    std::lock_guard lock(m_mutex);
    m_players.erase(player->playerID);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
    std::lock_guard lock(m_mutex);
    for (auto& p : m_players)
    {
        p.second->ownerSession->Send(sendBuffer);
    }
}
