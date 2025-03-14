#include "pch.h"
#include "protocol.h"
#include "Session.h"
#include "ServerPacketHandler.h"

using namespace Protocol;

PacketHandlerFunc GServerPacketHandler[UINT16_MAX];


bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
    return false;
}

bool Handle_DCS_TEST(SessionRef& session, BYTE* buffer, int32 len)
{
    DCS_TEST_PKT* pkt = reinterpret_cast<DCS_TEST_PKT*>(buffer);
    return false;
}
