#pragma once

using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GServerPacketHandler[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);

class ServerPacketHandler
{
public:
    static void Init()
    {
        for (int32 i = 0; i < UINT16_MAX; ++i) GServerPacketHandler[i] = Handle_INVALID;

    }


    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
        return GServerPacketHandler[header->id](session, buffer, len);
    }

};

