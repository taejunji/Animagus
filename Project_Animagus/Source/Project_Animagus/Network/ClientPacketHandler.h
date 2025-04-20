#pragma once
#include <functional>
#include "Types.h"
#include "../../../Server/Server/protocol.h"


#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "../Project_Animagus.h"
#endif

//using namespace Protocol;

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GClientPacketHandler[1024];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_DCS_TEST(SessionRef& session, Protocol::DCS_TEST_PKT& pkt);
bool Handle_SC_ENTER_GAME(SessionRef& session, Protocol::SC_ENTER_GAME_PKT& pkt);
bool Handle_SC_SPAWN(SessionRef& session, Protocol::SC_SPAWN_PKT& pkt);
bool Handle_CS_MOVE(SessionRef& session, Protocol::CS_MOVE_PKT& pkt);
bool Handle_CS_USING_SKILL(SessionRef& session, Protocol::CS_USING_SKILL_PKT& pkt);
bool Handle_SC_SPAWN_ITEM(SessionRef& session, Protocol::SC_SPAWN_ITEM_PKT& pkt);
bool Handle_SC_UPDATE_HP(SessionRef& session, Protocol::SC_UPDATE_HP_PKT& pkt);

class ClientPacketHandler
{
public:
    static void Init()
    {
        for (uint16 i = 0; i < 1024; ++i) GClientPacketHandler[i] = Handle_INVALID;
        GClientPacketHandler[(int32)Protocol::PacketID::DCS_TEST] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::DCS_TEST_PKT>(Handle_DCS_TEST, session, buffer, len); };
        GClientPacketHandler[(int32)Protocol::PacketID::SC_ENTER_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_ENTER_GAME_PKT>(Handle_SC_ENTER_GAME, session, buffer, len); };
        GClientPacketHandler[(int32)Protocol::PacketID::SC_SPAWN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_SPAWN_PKT>(Handle_SC_SPAWN, session, buffer, len); };
        GClientPacketHandler[(int32)Protocol::PacketID::CS_MOVE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_MOVE_PKT>(Handle_CS_MOVE, session, buffer, len); };
        GClientPacketHandler[(int32)Protocol::PacketID::CS_USING_SKILL] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::CS_USING_SKILL_PKT>(Handle_CS_USING_SKILL, session, buffer, len); };
        GClientPacketHandler[(int32)Protocol::PacketID::SC_SPAWN_ITEM] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_SPAWN_ITEM_PKT>(Handle_SC_SPAWN_ITEM, session, buffer, len); };
        GClientPacketHandler[(int32)Protocol::PacketID::SC_UPDATE_HP] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::SC_UPDATE_HP_PKT>(Handle_SC_UPDATE_HP, session, buffer, len); };

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
    static SendBufferRef MakeSendBuffer(Protocol::CS_USING_SKILL_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::CS_USING_SKILL); }
    static SendBufferRef MakeSendBuffer(Protocol::CS_AI_ENTER_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::CS_AI_ENTER); }
    static SendBufferRef MakeSendBuffer(Protocol::CS_AI_MOVE_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::CS_AI_MOVE); }
    static SendBufferRef MakeSendBuffer(Protocol::CS_AI_USING_SKILL_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::CS_AI_USING_SKILL); }
    static SendBufferRef MakeSendBuffer(Protocol::CS_DAMAGE_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)Protocol::PacketID::CS_DAMAGE); }

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

