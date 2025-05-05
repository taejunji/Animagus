#include "pch.h"
#include "protocol.h"
#include "Session.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "Room.h"

using namespace Protocol;

PacketHandlerFunc GServerPacketHandler[1024];


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

bool Handle_CS_ENTER_ROOM(SessionRef& session, CS_ENTER_ROOM_PKT& pkt)
{
    //Handle_CS_ENTER_GAME 내용 잘라서 여기 붙이기
    PlayerRef player = PlayerFactory::CreatePlayer(std::static_pointer_cast<Session>(session));

    //GRoom->Enter(player);
    GRoom[pkt.room_id]->HandleEnterPlayer(player);

#ifndef _DUMMYTEST
    std::cout << player->playerID << ": Enter Game" << std::endl;
#endif

    return true;
}

bool Handle_CS_START_GAME(SessionRef& session, CS_START_GAME_PKT& pkt)
{
    std::cout << "Host Request to Start Game" << std::endl;

    PlayerRef player = session->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    SC_START_GAME_PKT start_pkt;
    SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(start_pkt);
    room->Broadcast(sendBuffer, 0);

    return true;
}

bool Handle_CS_ENTER_GAME(SessionRef& session, CS_ENTER_GAME_PKT& pkt)
{
    PlayerRef player = session->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleStartGame(player);

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

bool Handle_CS_AI_USING_SKILL(SessionRef& session, CS_AI_USING_SKILL_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleAISkillLocked(pkt, player->playerID);

    return true;
}

bool Handle_CS_DAMAGE(SessionRef& session, CS_DAMAGE_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleDamageLocked(pkt, player->playerID);

    return true;
}

bool Handle_CS_TIME_OVER(SessionRef& session, CS_TIME_OVER_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleTimeOverLocked(pkt);

    return true;
}

// 브랜치 보호용 커밋
