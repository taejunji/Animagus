#include "pch.h"
#include "protocol.h"
#include "Session.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "DBManager.h"

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
    //std::cout << player->playerID << ": Enter Room#" << pkt.room_id << std::endl;
#endif

    return true;
}

bool Handle_CS_START_GAME(SessionRef& session, CS_START_GAME_PKT& pkt)
{

    PlayerRef player = session->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    std::cout << "Room#" << room->m_roomID << " Host Request to Start Game" << std::endl;

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

    room->HandleEnterGame();

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

bool Handle_CS_ROUND_END(SessionRef& session, CS_ROUND_END_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleRoundEndLocked(pkt);

    return true;
}

bool Handle_CS_ROUND_INIT(SessionRef& session, CS_ROUND_INIT_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleRoundInitLocked(pkt);

    return true;
}

bool Handle_CS_SELECT_CHARACTER(SessionRef& session, CS_SELECT_CHARACTER_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    player->type = pkt.p_type;

    std::cout << "Room#" << player->room.load().lock()->m_roomID << " Player[" << player->playerID << "] Selected Character Type: " << static_cast<int>(pkt.p_type) << std::endl;

    return true;
}

bool Handle_CS_SKILL_CHANGE(SessionRef& session, CS_SKILL_CHANGE_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleHitChangeSkill(pkt);

    return true;
}

bool Handle_CS_LOGIN(SessionRef& session, CS_LOGIN_PKT& pkt)
{
    std::cout << "Login ID: " << pkt.login_id << ", Password: " << pkt.login_pwd << std::endl;

    // DBManager::DBFindById(...);
    auto& instance = DBManager::GetInstance();

    char UserName[MAX_NAME_LEN + 1];
    char flag;
    if (true == instance.DBFindById(pkt.login_id, pkt.login_pwd, UserName, &flag))
    {
        std::string userNameStr(UserName);
        userNameStr.erase(remove(userNameStr.begin(), userNameStr.end(), ' '), userNameStr.end());
        std::cout << userNameStr << " LogIn Success" << std::endl;

        session->m_userId = pkt.login_id;
        session->m_userName = userNameStr;
        session->m_loggedIn = true;

        SC_LOGIN_SUCC_PKT succ_pkt;
        strcpy_s(succ_pkt.player_name, userNameStr.c_str());
        auto sendBuffer = ServerPacketHandler::MakeSendBuffer(succ_pkt);
        session->Send(sendBuffer);
    }
    else {
        SC_LOGIN_FAIL_PKT fail_pkt;
        fail_pkt.reason = flag;
        auto sendBuffer = ServerPacketHandler::MakeSendBuffer(fail_pkt);
        session->Send(sendBuffer);

        if (flag == Protocol::LOGIN_USING)
            std::cout << "Someone Using" << std::endl;
        else if (flag == Protocol::LOGIN_NOEX)
            std::cout << "NO Data in DB" << std::endl;
        else {
            std::cout << "Error" << std::endl;
        }
    }

    return true;
}

bool Handle_CS_SIGN_UP(SessionRef& session, CS_SIGN_UP_PKT& pkt)
{
    std::cout << "Signed ID: " << pkt.sign_id << ", Password: " << pkt.sign_pwd << ", Name: " << pkt.sign_name << std::endl;

    // DBManager::DBSignedUp(...);
    if (true == DBManager::GetInstance().DBSignUp(pkt.sign_id, pkt.sign_pwd, pkt.sign_name)) {
        SC_SIGNUP_SUCC_PKT succ_pkt;
        auto sendBuffer = ServerPacketHandler::MakeSendBuffer(succ_pkt);
        session->Send(sendBuffer);

        std::cout << "SignUp Success" << std::endl;
    }

    return true;
}

bool Handle_CS_JUMP_EFT(SessionRef& session, CS_JUMP_EFT_PKT& pkt)
{
    auto gameSession = static_pointer_cast<Session>(session);

    PlayerRef player = gameSession->m_player.load();
    if (player == nullptr)
        return false;

    RoomRef room = player->room.load().lock();
    if (room == nullptr)
        return false;

    room->HandleJumpEffect(pkt);

    return true;
}

