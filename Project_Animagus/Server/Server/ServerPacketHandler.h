#pragma once
#include "pch.h"
#include "protocol.h"

using namespace Protocol;

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GServerPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_DCS_TEST(SessionRef& session, DCS_TEST_PKT& pkt);

class ServerPacketHandler
{
public:
    static void Init()
    {
        for (uint16 i = 0; i < UINT16_MAX; ++i) GServerPacketHandler[i] = Handle_INVALID;
        GServerPacketHandler[(int32)PacketID::DCS_TEST] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<DCS_TEST_PKT>(Handle_DCS_TEST, session, buffer, len); };
    }


    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
        return GServerPacketHandler[(int32)header->id](session, buffer, len);
    }

private:
    template<typename PacketType, typename ProcessFunc>
    static bool HandlePacket(ProcessFunc func, SessionRef& session, BYTE* buffer, int32 len)
    {
        PacketType pkt;
        if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
            return false;

        return func(session, pkt);
    }

    template<typename T>
    static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
    {
        const uint16 dataSize = static_cast<uint16>(sizeof(pkt));
        const uint16 packetSize = dataSize + sizeof(PacketHeader);

        SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
        PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
        header->size = packetSize;
        header->id = pktId;
        if (pkt.SerializeToArray(&header[1], dataSize));
        sendBuffer->Close(packetSize);

        return sendBuffer;
    }


};

