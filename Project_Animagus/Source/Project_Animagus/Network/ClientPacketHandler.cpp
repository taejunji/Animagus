#include "ClientPacketHandler.h"

#include <string>
#include "Types.h"
#include "../Project_Animagus.h"
#include "Session.h"
#include "../System/MyGameInstance.h"
#include "../GameMode/BattleGameMode.h"

//using namespace Protocol;

PacketHandlerFunc GClientPacketHandler[UINT16_MAX];


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

bool Handle_SC_SPAWN_PKT(SessionRef& session, Protocol::SC_SPAWN_PKT& pkt)
{   // TEMP : 기존 플레이어 or 신입 플레이어 스폰해줘야 함

    if (auto* GameInstance = Cast<UMyGameInstance>(GWorld->GetGameInstance()))
    {
        GameInstance->HandleSpawn(pkt);
    }

    return true;
}
