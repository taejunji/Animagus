#pragma once
#include "pch.h"
#include "protocol.h"

using namespace Protocol;

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GServerPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_DCS_TEST(SessionRef& session, DCS_TEST_PKT& pkt);
bool Handle_CS_ENTER_GAME(SessionRef& session, CS_ENTER_GAME_PKT& pkt);
bool Handle_CS_LEAVE(SessionRef& session, CS_LEAVE_PKT& pkt);


class ServerPacketHandler
{
public:
    static void Init()
    {
        for (uint16 i = 0; i < UINT16_MAX; ++i) GServerPacketHandler[i] = Handle_INVALID;
        GServerPacketHandler[(int32)PacketID::DCS_TEST] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<DCS_TEST_PKT>(Handle_DCS_TEST, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_ENTER_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_ENTER_GAME_PKT>(Handle_CS_ENTER_GAME, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_LEAVE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_LEAVE_PKT>(Handle_CS_LEAVE, session, buffer, len); };
    }

    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
        return GServerPacketHandler[(int32)header->id](session, buffer, len);
    }

    static SendBufferRef MakeSendBuffer(DCS_TEST_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::DCS_TEST); }
    static SendBufferRef MakeSendBuffer(SC_SPAWN_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_SPAWN); }

private:
    template<typename PacketType, typename ProcessFunc>
    static bool HandlePacket(ProcessFunc func, SessionRef& session, BYTE* buffer, int32 len)
    {
        PacketType pkt;

        const BYTE* pktPtr = buffer + sizeof(PacketHeader);
        const uint16 dataSize = len - sizeof(PacketHeader);

        if (dataSize < sizeof(PacketType)) return false;

        std::memcpy(&pkt, pktPtr, sizeof(PacketType));

        return func(session, pkt);
    }

    template<typename T>
    static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
    {
        const uint16 dataSize = static_cast<uint16>(sizeof(pkt));
        const uint16 packetSize = dataSize + sizeof(PacketHeader);

        SendBufferRef sendBuffer = std::make_shared<SendBuffer>(new BYTE[packetSize], packetSize);
        PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
        header->size = packetSize;
        header->id = pktId;
        std::memcpy(&header[1], &pkt, dataSize);    // 헤더 바로 뒤에 패킷 데이터 복사
        sendBuffer->Close(packetSize);              // 버퍼 사용량 설정 (_writeSize)

        return sendBuffer;
    }


};

