#include "pch.h"
#include "IocpEvent.h"
#include "IocpCore.h"
#include "Session.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "GameServer.h"

/*--------------
    Listener
---------------*/

Listener::~Listener()
{
    ::closesocket(_socket);

    for (IocpEvent* acceptEvent : _acceptEvents)
    {
        // TODO : 이벤트들 메모리 해제

    }
}

bool Listener::StartAccept(GameServerRef server)
{
    _server = server;
    if (_server == nullptr)
        return false;

    _socket = SocketUtils::CreateSocket();
    if (_socket == INVALID_SOCKET)
        return false;

    if (_server->GetIocpCore()->Register(shared_from_this()) == false)
        return false;

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::SetLinger(_socket, 0, 0) == false)
        return false;

    if (SocketUtils::BindAnyAddress(_socket) == false)
        return false;

    if (SocketUtils::Listen(_socket) == false)
        return false;

    const int32 acceptCount = MAX_USER;
    for (int32 i = 0; i < acceptCount; i++)
    {
        AcceptEvent* acceptEvent = new AcceptEvent();
        acceptEvent->owner = shared_from_this();
        _acceptEvents.push_back(acceptEvent);
        RegisterAccept(acceptEvent);
    }

    return true;
}

void Listener::CloseSocket()
{
    SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    if (iocpEvent->eventType != EventType::Accept) return;
    AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    // Session 생성, CP에 등록 및 서버에 해당 세션 추가
    SessionRef session = std::make_shared<Session>();
    session->SetService(_server);
    _server->GetIocpCore()->Register(session);

    acceptEvent->Init();
    acceptEvent->session = session;

    DWORD bytesReceived = 0;
    if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->m_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
    {
        const int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            // 일단 다시 Accept 걸어준다
            RegisterAccept(acceptEvent);
        }
    }
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
    SessionRef session = acceptEvent->session;

    if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    SOCKADDR_IN sockAddress;
    int32 sizeOfSockAddr = sizeof(sockAddress);
    if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
    {
        RegisterAccept(acceptEvent);
        return;
    }

    session->SetNetAddress(sockAddress);
    session->ProcessConnect();
    RegisterAccept(acceptEvent);    // 물고기 어망에 담고 다시 낚싯대 던지기
}