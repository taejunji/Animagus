#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "Buffers.h"
#include "Session.h"


void Room::Enter(PlayerRef player)
{
    std::lock_guard lock(m_mutex);
    m_players[player->playerID] = player;
    m_playerCount++;
}

void Room::Leave(PlayerRef player)
{
    std::lock_guard lock(m_mutex);
    m_players.erase(player->playerID);
    m_playerCount++;
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
    std::lock_guard lock(m_mutex);
    for (auto& p : m_players)
    {
        p.second->ownerSession->Send(sendBuffer);;
    }
}
