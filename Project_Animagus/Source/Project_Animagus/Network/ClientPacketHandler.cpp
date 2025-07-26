#include "ClientPacketHandler.h"

#include <string>
#include "Types.h"
#include "../Project_Animagus.h"
#include "Session.h"
#include "../System/MyGameInstance.h"
#include "../GameMode/BattleGameMode.h"

//using namespace Protocol;

PacketHandlerFunc GClientPacketHandler[1024];


bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
    return false;
}

bool Handle_DCS_TEST(SessionRef& session, Protocol::DCS_TEST_PKT& pkt)
{
    FString MsgStr = FString(ANSI_TO_TCHAR(pkt.msg)).Left((int32)pkt.len);
    UE_LOG(LogTemp, Warning, TEXT("Message: %s"), *MsgStr);
    return true;
}

bool Handle_SC_UR_HOST(SessionRef& session, Protocol::SC_UR_HOST_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleLobbyHost(pkt);
    }

    return true;
}

bool Handle_SC_START_GAME(SessionRef& session, Protocol::SC_START_GAME_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleStartGame(pkt);
    }

    return true;
}

bool Handle_SC_ENTER_GAME(SessionRef& session, Protocol::SC_ENTER_GAME_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleEnterGame(pkt);
    }

    return true;
}

bool Handle_SC_SPAWN(SessionRef& session, Protocol::SC_SPAWN_PKT& pkt)
{   // TEMP : 기존 플레이어 or 신입 플레이어 스폰해줘야 함

    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSpawn(pkt);
    }

    return true;
}

bool Handle_CS_MOVE(SessionRef& session, Protocol::CS_MOVE_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleMove(pkt);
    }

    return true;
}

bool Handle_CS_USING_SKILL(SessionRef& session, Protocol::CS_USING_SKILL_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSkill(pkt);
    }

    return true;
}

bool Handle_SC_SPAWN_ITEM(SessionRef& session, Protocol::SC_SPAWN_ITEM_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSpawnItem(pkt);
    }

    return true;
}

bool Handle_SC_UPDATE_HP(SessionRef& session, Protocol::SC_UPDATE_HP_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleUpdateHp(pkt);
    }

    return true;
}

bool Handle_SC_ROUND_END(SessionRef& session, Protocol::SC_ROUND_END_PKT& pkt)
{
    UE_LOG(LogTemp, Warning, TEXT("DoEnd BattleGameMode"));

    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleBattleRoundEnd(pkt);
    }

    return true;
}

bool Handle_SC_ROUND_INIT(SessionRef& session, Protocol::SC_ROUND_INIT_PKT& pkt)
{
    UE_LOG(LogTemp, Warning, TEXT("Init BattleGameMode"));

    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleBattleRoundInit(pkt);
    }

    return true;
}

bool Handle_SC_AI_SPAWN(SessionRef& session, Protocol::SC_AI_SPAWN_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleAISpawn(pkt);
    }

    return true;
}

bool Handle_CS_JUMP_EFT(SessionRef& session, Protocol::CS_JUMP_EFT_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleJumpEffect(pkt);
    }

    return true;
}

bool Handle_SC_GAME_END(SessionRef& session, Protocol::SC_GAME_END_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleBattleModeEnd(pkt);
    }

    return true;
}

bool Handle_SC_LOGIN_SUCC(SessionRef& session, Protocol::SC_LOGIN_SUCC_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleLoginSuccess(pkt);
    }

    return true;
}

bool Handle_SC_LOGIN_FAIL(SessionRef& session, Protocol::SC_LOGIN_FAIL_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleLoginFail(pkt);
    }
    
    return true;
}

bool Handle_SC_SIGNUP_SUCC(SessionRef& session, Protocol::SC_SIGNUP_SUCC_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSignUpSuccess();
    }

    return true;
}

bool Handle_SC_SIGNUP_FAIL(SessionRef& session, Protocol::SC_SIGNUP_FAIL_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSignUpFail();
    }

    return true;
}

bool Handle_SC_SET_POWERUP(SessionRef& session, Protocol::SC_SET_POWERUP_PKT& pkt)
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSetPowerUp(pkt);
    }

    return true;
}
