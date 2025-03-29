#include "pch.h"
#include "protocol.h"
#include "Session.h"
#include "Player.h"


std::atomic<int64> PlayerFactory::s_idGenerator = 0;

PlayerRef PlayerFactory::CreatePlayer(SessionRef session)
{
    // ID 생성기
    const int64 newId = s_idGenerator.fetch_add(1);

    PlayerRef player = std::make_shared<Player>();
    player->playerID = newId;

    player->ownerSession = session;
    session->m_player.store(player);

    return player;
}
