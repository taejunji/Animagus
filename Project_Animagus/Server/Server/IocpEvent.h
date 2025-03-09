#pragma once


class Session;

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

class AcceptEvent : public IocpEvent
{
public:
    AcceptEvent() : IocpEvent(EventType::Accept) {}

    //void		SetSession(Session* session) { _session = session; }
    //Session*	GetSession() { return _session; }

public:
    SessionRef	session = nullptr;		// 클라세션
};

