#pragma once
#include "Types.h"
#include "IocpCore.h"


enum class EventType : uint8
{
    Connect,
    Disconnect,
    Accept,
    //PreRecv,
    Recv,
    Send
};


class IocpEvent : public OVERLAPPED
{
public:
    IocpEvent(EventType type);

    void		Init();
    //EventType	GetType() { return eventType; }

public:
    EventType		eventType;
    IocpObjectRef	owner;
};

