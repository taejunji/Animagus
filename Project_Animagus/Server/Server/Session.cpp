#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
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
    case EventType::Connect:
        ProcessConnect();
        break;
    case EventType::Disconnect:
        ProcessDisconnect();
        break;
    case EventType::Recv:
        ProcessRecv(numOfBytes);
        break;
    case EventType::Send:
        ProcessSend(numOfBytes);
        break;
    default: break;
    }
}

void Session::Send(BYTE* buffer, int32 len)
{

    RegisterSend();
}

bool Session::Connect()
{
    return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
    if (m_connected.exchange(false) == false)
        return;

    // TEMP
    std::wcout << "Disconnect : " << cause << std::endl;

    RegisterDisconnect();
}

bool Session::RegisterConnect()
{
    if (IsConnected())
        return false;

    if (SocketUtils::SetReuseAddress(m_socket, true) == false)
        return false;

    if (SocketUtils::BindAnyAddress(m_socket/*남는거*/) == false)
        return false;

    _connectEvent.Init();
    _connectEvent.owner = shared_from_this(); // ADD_REF

    DWORD numOfBytes = 0;
    SOCKADDR_IN sockAddr = m_sockAddress;
    if (false == SocketUtils::ConnectEx(m_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            _connectEvent.owner = nullptr; // RELEASE_REF
            return false;
        }
    }

    return true;
}

bool Session::RegisterDisconnect()
{
    _disconnectEvent.Init();
    _disconnectEvent.owner = shared_from_this(); // ADD_REF

    if (false == SocketUtils::DisconnectEx(m_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            _disconnectEvent.owner = nullptr; // RELEASE_REF
            return false;
        }
    }

    return true;
}

void Session::RegisterRecv()
{
    if (IsConnected() == false)
        return;

    _recvEvent.Init();
    _recvEvent.owner = shared_from_this(); // ADD_REF

    // TODO : wsabuf 관련 설정
    //WSABUF wsaBuf;
    //wsaBuf.buf = reinterpret_cast<char*>(m_buffer);
    //wsaBuf.len = m_buffer.FreeSize();

    DWORD numOfBytes = 0;
    DWORD flags = 0;
    if (SOCKET_ERROR == ::WSARecv(m_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            HandleError(errorCode);
            _recvEvent.owner = nullptr; // RELEASE_REF
        }
    }
}


void Session::ProcessRecv(int32 numOfBytes)
{
    std::wcout << "Recv: " << numOfBytes << " bytes received." << std::endl;
    // m_buffer 또는 m_wsaBuf.buf 에 채워진 데이터를 파싱/처리하는 로직 구현
}

// 전송 이벤트 처리: 전송 완료 후 후속 작업 수행
void Session::ProcessSend(int32 numOfBytes)
{
    std::wcout << "Send: " << numOfBytes << " bytes sent." << std::endl;
    // 전송 완료 후 필요한 로직 구현
}


void Session::HandleError(int32 errorCode)
{
    switch (errorCode)
    {
    case WSAECONNRESET:
    case WSAECONNABORTED:
        Disconnect(L"HandleError");
        break;
    default:
        // TODO : Log
        cout << "Handle Error : " << errorCode << endl;
        break;
    }
}
