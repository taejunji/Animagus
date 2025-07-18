#pragma once
#include "pch.h"
#include "protocol.h"

using namespace Protocol;

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GServerPacketHandler[1024];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_DCS_TEST(SessionRef& session, DCS_TEST_PKT& pkt);
bool Handle_CS_ENTER_ROOM(SessionRef& session, CS_ENTER_ROOM_PKT& pkt);
bool Handle_CS_START_GAME(SessionRef& session, CS_START_GAME_PKT& pkt);
bool Handle_CS_ENTER_GAME(SessionRef& session, CS_ENTER_GAME_PKT& pkt);
bool Handle_CS_LEAVE(SessionRef& session, CS_LEAVE_PKT& pkt);
bool Handle_CS_MOVE(SessionRef& session, CS_MOVE_PKT& pkt);
bool Handle_CS_USING_SKILL(SessionRef& session, CS_USING_SKILL_PKT& pkt);
bool Handle_CS_AI_ENTER(SessionRef& session, CS_AI_ENTER_PKT& pkt);
bool Handle_CS_AI_MOVE(SessionRef& session, CS_AI_MOVE_PKT& pkt);
bool Handle_CS_AI_USING_SKILL(SessionRef& session, CS_AI_USING_SKILL_PKT& pkt);
bool Handle_CS_DAMAGE(SessionRef& session, CS_DAMAGE_PKT& pkt);
bool Handle_CS_ROUND_END(SessionRef& session, CS_ROUND_END_PKT& pkt);
bool Handle_CS_ROUND_INIT(SessionRef& session, CS_ROUND_INIT_PKT& pkt);
bool Handle_CS_SELECT_CHARACTER(SessionRef& session, CS_SELECT_CHARACTER_PKT& pkt);
bool Handle_CS_SKILL_CHANGE(SessionRef& session, CS_SKILL_CHANGE_PKT& pkt);
bool Handle_CS_LOGIN(SessionRef& session, CS_LOGIN_PKT& pkt);
bool Handle_CS_SIGN_UP(SessionRef& session, CS_SIGN_UP_PKT& pkt);
bool Handle_CS_JUMP_EFT(SessionRef& session, CS_JUMP_EFT_PKT& pkt);

class ServerPacketHandler
{
public:
    static void Init()
    {
        for (uint16 i = 0; i < 1024; ++i) GServerPacketHandler[i] = Handle_INVALID;
        GServerPacketHandler[(int32)PacketID::DCS_TEST] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<DCS_TEST_PKT>(Handle_DCS_TEST, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_ENTER_ROOM] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_ENTER_ROOM_PKT>(Handle_CS_ENTER_ROOM, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_START_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_START_GAME_PKT>(Handle_CS_START_GAME, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_ENTER_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_ENTER_GAME_PKT>(Handle_CS_ENTER_GAME, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_LEAVE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_LEAVE_PKT>(Handle_CS_LEAVE, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_MOVE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_MOVE_PKT>(Handle_CS_MOVE, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_USING_SKILL] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_USING_SKILL_PKT>(Handle_CS_USING_SKILL, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_AI_ENTER] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_AI_ENTER_PKT>(Handle_CS_AI_ENTER, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_AI_MOVE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_AI_MOVE_PKT>(Handle_CS_AI_MOVE, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_AI_USING_SKILL] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_AI_USING_SKILL_PKT>(Handle_CS_AI_USING_SKILL, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_DAMAGE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_DAMAGE_PKT>(Handle_CS_DAMAGE, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_ROUND_END] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_ROUND_END_PKT>(Handle_CS_ROUND_END, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_ROUND_INIT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_ROUND_INIT_PKT>(Handle_CS_ROUND_INIT, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_SELECT_CHARACTER] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_SELECT_CHARACTER_PKT>(Handle_CS_SELECT_CHARACTER, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_SKILL_CHANGE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_SKILL_CHANGE_PKT>(Handle_CS_SKILL_CHANGE, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_LOGIN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_LOGIN_PKT>(Handle_CS_LOGIN, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_SIGN_UP] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_SIGN_UP_PKT>(Handle_CS_SIGN_UP, session, buffer, len); };
        GServerPacketHandler[(int32)PacketID::CS_JUMP_EFT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<CS_JUMP_EFT_PKT>(Handle_CS_JUMP_EFT, session, buffer, len); };

    }

    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
        return GServerPacketHandler[(int32)header->id](session, buffer, len);
    }

    static SendBufferRef MakeSendBuffer(DCS_TEST_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::DCS_TEST); }
    static SendBufferRef MakeSendBuffer(SC_SPAWN_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_SPAWN); }
    static SendBufferRef MakeSendBuffer(SC_UR_HOST_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_UR_HOST); }
    static SendBufferRef MakeSendBuffer(SC_START_GAME_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_START_GAME); }
    static SendBufferRef MakeSendBuffer(SC_ENTER_GAME_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_ENTER_GAME); }
    static SendBufferRef MakeSendBuffer(CS_MOVE_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::CS_MOVE); }
    static SendBufferRef MakeSendBuffer(CS_USING_SKILL_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::CS_USING_SKILL); }
    static SendBufferRef MakeSendBuffer(SC_SPAWN_ITEM_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_SPAWN_ITEM); }
    static SendBufferRef MakeSendBuffer(SC_UPDATE_HP_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_UPDATE_HP); }
    static SendBufferRef MakeSendBuffer(SC_ROUND_END_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_ROUND_END); }
    static SendBufferRef MakeSendBuffer(SC_AI_SPAWN_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_AI_SPAWN); }
    static SendBufferRef MakeSendBuffer(SC_ROUND_INIT_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_ROUND_INIT); }
    static SendBufferRef MakeSendBuffer(CS_JUMP_EFT_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::CS_JUMP_EFT); }
    static SendBufferRef MakeSendBuffer(SC_GAME_END_PKT& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_GAME_END); }
    static SendBufferRef MakeSendBuffer(SC_LOGIN_SUCC& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_LOGIN_SUCC); }
    static SendBufferRef MakeSendBuffer(SC_LOGIN_FAIL& pkt) { return MakeSendBuffer(pkt, (uint16)PacketID::SC_LOGIN_FAIL); }

    
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

