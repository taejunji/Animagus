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

bool Handle_DCS_TEST(SessionRef& session, DCS_TEST_PKT& pkt)
{
    std::cout << std::string(pkt.msg, pkt.len) << std::endl;
    return true;
}
