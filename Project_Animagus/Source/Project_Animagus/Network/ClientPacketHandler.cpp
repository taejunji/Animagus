#include "ClientPacketHandler.h"

#include <string>
#include "Types.h"
#include "../Project_Animagus.h"
#include "Session.h"
using namespace Protocol;

PacketHandlerFunc GClientPacketHandler[UINT16_MAX];


bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
    return false;
}

bool Handle_DCS_TEST(SessionRef& session, DCS_TEST_PKT& pkt)
{
    FString MsgStr = FString(UTF8_TO_TCHAR(pkt.msg)).Left((int32)pkt.len);
    UE_LOG(LogTemp, Log, TEXT("Message: %s"), *MsgStr);
    return true;
}
