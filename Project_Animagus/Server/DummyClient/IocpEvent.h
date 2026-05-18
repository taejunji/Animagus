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


/*----------------
    ConnectEvent
-----------------*/

class ConnectEvent : public IocpEvent
{
public:
    ConnectEvent() : IocpEvent(EventType::Connect) {}
};

/*--------------------
    DisconnectEvent
----------------------*/

class DisconnectEvent : public IocpEvent
{
public:
    DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};

/*----------------
    AcceptEvent
-----------------*/

class AcceptEvent : public IocpEvent
{
public:
    AcceptEvent() : IocpEvent(EventType::Accept) {}

    //void		SetSession(Session* session) { _session = session; }
    //Session*	GetSession() { return _session; }

public:
    SessionRef	session = nullptr;		// 클라세션
};

/*----------------
    RecvEvent
-----------------*/

class RecvEvent : public IocpEvent
{
public:
    RecvEvent() : IocpEvent(EventType::Recv) {}
};

/*----------------
    SendEvent
-----------------*/

class SendEvent : public IocpEvent
{
public:
    SendEvent() : IocpEvent(EventType::Send) {}

    //std::string buffer;
    std::vector<SendBufferRef> sendBuffers;     // WSASend 는 여러개 모아서 한 번에 보내는게 좋은 효율
};