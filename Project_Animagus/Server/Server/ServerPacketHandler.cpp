#include "pch.h"
#include "protocol.h"
#include "Session.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "Room.h"

using namespace Protocol;

PacketHandlerFunc GServerPacketHandler[UINT16_MAX];


bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
    return false;
}

bool Handle_DCS_TEST(SessionRef& session, DCS_TEST_PKT& pkt)
{
    //std::cout << std::string(pkt.msg, pkt.len) << std::endl;

    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
    session->Send(sendBuffer);

    return true;
}

bool Handle_CS_ENTER_GAME(SessionRef& session, CS_ENTER_GAME_PKT& pkt)
{
    PlayerRef player = PlayerFactory::CreatePlayer(std::static_pointer_cast<Session>(session));

    //GRoom->Enter(player);
    GRoom->HandleEnterPlayer(player);

    std::cout << player->playerID << ": Enter Game" << std::endl;

    return true;
}

bool Handle_CS_LEAVE(SessionRef& session, CS_LEAVE_PKT& pkt)
{
    PlayerRef player = session->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleLeavePlayer(player);

    return true;
}

bool Handle_CS_MOVE(SessionRef& session, CS_MOVE_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleMoveLocked(pkt);

    return true;
}

bool Handle_CS_USING_SKILL(SessionRef& session, CS_USING_SKILL_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleSkillLocked(pkt);

    return true;
}

bool Handle_CS_AI_ENTER(SessionRef& session, CS_AI_ENTER_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleEnterAIPlayer(pkt);

    return true;
}

bool Handle_CS_AI_MOVE(SessionRef& session, CS_AI_MOVE_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleAIMoveLocked(pkt, player->playerID);

    return true;
}
