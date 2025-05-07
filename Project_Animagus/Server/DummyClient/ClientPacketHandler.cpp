#include "pch.h"
#include "ClientPacketHandler.h"
#include "Buffers.h"
#include "Session.h"
#include "ClientService.h"

using namespace Protocol;

PacketHandlerFunc GClientPacketHandler[1024];
std::atomic<uint64> GLatency = 0;

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

bool Handle_SC_ENTER_GAME(SessionRef& session, SC_ENTER_GAME_PKT& pkt)
{
    if (session == nullptr) return false;

    session->playerID = pkt.player_id;
    GClientCount++;

    if (GClientCount >= session->GetService()->CLIENT_COUNT)
    {
        std::cout << "[Client] All clients connected. cnt: " << GClientCount << std::endl;
    }

    return true;
}

bool Handle_CS_MOVE(SessionRef& session, CS_MOVE_PKT& pkt)
{
    if (session == nullptr) return false;
    if (session->playerID != pkt.player_info.player_id) return false;
    uint64 now = GetTickCount64();
    uint64 latency = now - session->client_timer;

    GLatency += (latency - GLatency) / GClientCount;
}