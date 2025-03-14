#pragma once
#include "pch.h"
#include "protocol.h"

using namespace Protocol;

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GServerPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_DCS_TEST(SessionRef& session, BYTE* buffer, int32 len);

class ServerPacketHandler
{
public:
    static void Init()
    {
        for (uint16 i = 0; i < UINT16_MAX; ++i) GServerPacketHandler[i] = Handle_INVALID;
        GServerPacketHandler[(int32)PacketID::DCS_TEST] = Handle_DCS_TEST;
    }


    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
        return GServerPacketHandler[(int32)header->id](session, buffer, len);
    }

};

