#pragma once
#include <functional>
#include "Types.h"
#include "../../../Server/Server/protocol.h"


#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "../Project_Animagus.h"
#endif

//using namespace Protocol;

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GClientPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_DCS_TEST(SessionRef& session, Protocol::DCS_TEST_PKT& pkt);
bool Handle_SC_SPAWN_PKT(SessionRef& session, Protocol::SC_SPAWN_PKT& pkt);

class ClientPacketHandler
{
public:
    static void Init()
    {
        for (uint16 i = 0; i < UINT16_MAX; ++i) GClientPacketHandler[i] = Handle_INVALID;
        GClientPacketHandler[(int32)Protocol::PacketID::DCS_TEST] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::DCS_TEST_PKT>(Handle_DCS_TEST, session, buffer, len); };
        GClientPacketHandler[(int32)Protocol::PacketID::SC_SPAWN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_SPAWN_PKT>(Handle_SC_SPAWN_PKT, session, buffer, len); };

    }


    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
        return GClientPacketHandler[(int32)header->id](session, buffer, len);
    }

    static SendBufferRef MakeSendBuffer(Protocol::DCS_TEST_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::DCS_TEST); }
    static SendBufferRef MakeSendBuffer(Protocol::CS_LEAVE_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::CS_LEAVE); }
    static SendBufferRef MakeSendBuffer(Protocol::CS_ENTER_GAME_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::CS_ENTER_GAME); }
    static SendBufferRef MakeSendBuffer(Protocol::CS_MOVE_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::CS_MOVE); }


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

        SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
        PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
        header->size = packetSize;
        header->id = pktId;
        std::memcpy(&header[1], &pkt, dataSize);    // 헤더 바로 뒤에 패킷 데이터 복사
        sendBuffer->Close(packetSize);              // 버퍼 사용량 설정 (_writeSize)

        return sendBuffer;
    }


};

