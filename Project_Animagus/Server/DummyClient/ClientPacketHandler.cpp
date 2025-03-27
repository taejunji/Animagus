#include "pch.h"
#include "ClientPacketHandler.h"
#include "Buffers.h"
#include "Session.h"
using namespace Protocol;

PacketHandlerFunc GClientPacketHandler[UINT16_MAX];


bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
    return false;
}

bool Handle_DCS_TEST(SessionRef& session, DCS_TEST_PKT& pkt)
{
    //std::cout << std::string(pkt.msg, pkt.len) << std::endl;

    SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
    session->Send(sendBuffer);

    //std::this_thread::sleep_for(std::chrono::seconds(1));

    return true;
}

bool Handle_SC_SPAWN(SessionRef& session, SC_SPAWN_PKT& pkt)
{
    return true;
}
