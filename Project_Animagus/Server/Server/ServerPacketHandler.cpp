#include "pch.h"
#include "Session.h"
#include "ServerPacketHandler.h"

PacketHandlerFunc GServerPacketHandler[UINT16_MAX];


bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
    return false;
}
