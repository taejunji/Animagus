#include "Types.h"
#include "IocpEvent.h"
#include "IocpCore.h"


IocpEvent::IocpEvent(EventType type) : eventType(type)
{
    Init();
}

void IocpEvent::Init()
{
    OVERLAPPED::hEvent = 0;
    OVERLAPPED::Internal = 0;
    OVERLAPPED::InternalHigh = 0;
    OVERLAPPED::Offset = 0;
    OVERLAPPED::OffsetHigh = 0;
}
