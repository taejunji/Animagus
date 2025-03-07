#include "pch.h"
#include "IocpEvent.h"
#include "IocpCore.h"
#include "Session.h"
#include "SocketUtils.h"

/*--------------
	Session
---------------*/

Session::Session() : m_socket(INVALID_SOCKET) , m_connected(false) , m_buffer(nullptr)
{
    // 64KB 크기의 버퍼 할당
    m_buffer = new char[BUFFER_SIZE];
    m_wsaBuf.buf = m_buffer;
    m_wsaBuf.len = static_cast<ULONG>(BUFFER_SIZE);
}

// 소멸자: 소켓 닫기 및 버퍼 해제
Session::~Session()
{
    if (m_socket != INVALID_SOCKET)
    {
        SocketUtils::Close(m_socket);
    }
    delete[] m_buffer;
}

HANDLE Session::GetHandle()
{
    return reinterpret_cast<HANDLE>(m_socket);
}

// Dispatch(): IocpEvent의 이벤트 타입에 따라 처리 함수를 호출
void Session::Dispatch(IocpEvent* iocpEvent, int32_t numOfBytes)
{
    switch (iocpEvent->eventType)
    {
    case EventType::Recv:
        ProcessRecv(numOfBytes);
        break;
    case EventType::Send:
        ProcessSend(numOfBytes);
        break;
    case EventType::Accept:
        ProcessAccept(numOfBytes);
        break;
    default:
        // 필요한 경우 다른 이벤트에 대한 처리 추가
        break;
    }
}

void Session::ProcessRecv(int32_t numOfBytes)
{
    std::cout << "Recv: " << numOfBytes << " bytes received." << std::endl;
    // m_buffer 또는 m_wsaBuf.buf 에 채워진 데이터를 파싱/처리하는 로직 구현
}

// 전송 이벤트 처리: 전송 완료 후 후속 작업 수행
void Session::ProcessSend(int32_t numOfBytes)
{
    std::cout << "Send: " << numOfBytes << " bytes sent." << std::endl;
    // 전송 완료 후 필요한 로직 구현
}

// Accept 이벤트 처리: 클라이언트 연결 수락 후 처리
void Session::ProcessAccept(int32_t numOfBytes)
{
    m_connected.store(true);
    std::cout << "Accept completed. Client connected." << std::endl;
    // 추가적으로 클라이언트 접속 후 초기화 작업 등을 수행
}


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

bool Listener::StartAccept()
{
    _service = service;
    if (_service == nullptr)
        return false;

    _socket = SocketUtils::CreateSocket();
    if (_socket == INVALID_SOCKET)
        return false;

    if (_service->GetIocpCore()->Register(shared_from_this()) == false)
        return false;

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::SetLinger(_socket, 0, 0) == false)
        return false;

    if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
        return false;

    if (SocketUtils::Listen(_socket) == false)
        return false;

    const int32 acceptCount = _service->GetMaxSessionCount();
    for (int32 i = 0; i < acceptCount; i++)
    {
        IocpEvent* acceptEvent = xnew<AcceptEvent>();
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

bool Listener::BindAnyAddess()
{
    SOCKADDR_IN myAddress;
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
    myAddress.sin_port = ::htons(SERVER_PORT);

    return SOCKET_ERROR != ::bind(_socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

bool Listener::Listen(int32 backlog)
{
    return SOCKET_ERROR != ::listen(_socket, backlog);
}


HANDLE Listener::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    ASSERT_CRASH(iocpEvent->eventType == EventType::Accept);
    AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
    ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
    SessionRef session = _service->CreateSession(); // Register IOCP

    acceptEvent->Init();
    acceptEvent->session = session;

    DWORD bytesReceived = 0;
    if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
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

    session->SetNetAddress(NetAddress(sockAddress));
    session->ProcessConnect();
    RegisterAccept(acceptEvent);
}