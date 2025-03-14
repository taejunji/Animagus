#include "pch.h"
#include "Session.h"
#include "Buffers.h"
#include "IocpEvent.h"
#include "IocpCore.h"
#include "SocketUtils.h"
#include "GameServer.h"
#include "ServerPacketHandler.h"


/*--------------
	Session
---------------*/

Session::Session(ServiceType type) : m_serviceType(type), m_recvBuffer(BUFFER_SIZE)
{
    m_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
    if (m_socket != INVALID_SOCKET)
    {
        SocketUtils::Close(m_socket);
    }
    //delete[] m_sendBuffer;
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

    if (SocketUtils::BindAnyAddress(m_socket) == false)
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

    if (false == SocketUtils::DisconnectEx(m_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))    // 소켓 재샤용
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

// TODO: sendbuffer를 모아서 보내는 형식으로 함 도전?
void Session::Send(SendBufferRef sendBuffer)
{
    if (IsConnected() == false)
        return;

    bool registerSend = false;

    // 현재 RegisterSend가 걸리지 않은 상태라면, 걸어준다
    {
        std::lock_guard lock(m_mutex);

        m_sendBuffers.push(sendBuffer);

        if (m_sendRegistered.exchange(true) == false)
            registerSend = true;
    }

    if (registerSend)
        RegisterSend();
}


void Session::RegisterRecv()
{
    if (IsConnected() == false)
        return;

    _recvEvent.Init();
    _recvEvent.owner = shared_from_this(); // ADD_REF

    // TODO : wsabuf 관련 설정
    WSABUF wsaBuf;
    wsaBuf.buf = reinterpret_cast<char*>(m_recvBuffer.WritePos());
    wsaBuf.len = m_recvBuffer.FreeSize();

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

void Session::RegisterSend()
{
    if (IsConnected() == false)
        return;

    _sendEvent.Init();
    _sendEvent.owner = shared_from_this();	// ADD_REF

    // 보낼 데이터를 sendEvent에 등록
    {
        std::lock_guard lock(m_mutex);

        int32 writeSize = 0;
        while (m_sendBuffers.empty() == false)
        {
            SendBufferRef sendBuffer = m_sendBuffers.front();

            writeSize += sendBuffer->WriteSize();
            // TODO : Check Exception

            m_sendBuffers.pop();
            _sendEvent.sendBuffers.push_back(sendBuffer);
        }
    }

    // Scatter-Gather (흩어져 있는 데이터들을 모아서 한번에 보냄)
    std::vector<WSABUF> wsaBufs;
    wsaBufs.reserve(_sendEvent.sendBuffers.size());
    for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
    {
        WSABUF wsaBuf;
        wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
        wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());  // pkt_size
        wsaBufs.emplace_back(wsaBuf);
    }

    DWORD numOfBytes = 0;
    if (SOCKET_ERROR == ::WSASend(m_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
    {
        int32 errorCode = ::WSAGetLastError();
        if (errorCode != WSA_IO_PENDING)
        {
            HandleError(errorCode);
            _sendEvent.owner = nullptr;		// RELEASE_REF
            _sendEvent.sendBuffers.clear();	// RELEASE_REF
            m_sendRegistered.store(false);
        }
    }

}


void Session::ProcessConnect()
{
    _connectEvent.owner = nullptr;		// RELEASE_REF

    m_connected.store(true);

    // 세션 등록
    GetService()->AddSession(GetSessionRef());

    // 컨텐츠 코드에서 재정의
    OnConnected();

    // 수신 등록
    RegisterRecv();
}

void Session::ProcessDisconnect()
{
    _disconnectEvent.owner = nullptr;	// RELEASE_REF

    OnDisconnected();		// 컨텐츠 코드에서 재정의
    //SocketUtils::Close(_socket);
    GetService()->ReleaseSession(GetSessionRef());
}

void Session::ProcessRecv(int32 numOfBytes)
{
    _recvEvent.owner = nullptr;		// RELEASE_REF

    if (numOfBytes == 0)
    {
        // 연결이 끊긴 상황
        Disconnect(L"Recv 0");
        return;
    }

    if (m_recvBuffer.OnWrite(numOfBytes) == false)
    {
        Disconnect(L"OnWrite Overflow");
        return;
    }

    int32 dataSize = m_recvBuffer.DataSize();
    int32 processLen = OnRecv(m_recvBuffer.ReadPos(), dataSize); // 컨텐츠 코드에서 재정의
    if (processLen < 0 || dataSize < processLen || m_recvBuffer.OnRead(processLen) == false)
    {
        Disconnect(L"OnRead Overflow");
        return;
    }

    // 커서 정리
    m_recvBuffer.Clean();

    // 다시 수신 등록
    RegisterRecv();
}

// 전송 이벤트 처리: 전송 완료 후 후속 작업 수행
void Session::ProcessSend(int32 numOfBytes)
{
    _sendEvent.owner = nullptr;		// RELEASE_REF
    _sendEvent.sendBuffers.clear();	// RELEASE_REF

    if (numOfBytes == 0)
    {
        Disconnect(L"Send 0");
        return;
    }

    OnSend(numOfBytes);

    {
        std::lock_guard lock(m_mutex);

        if (m_sendBuffers.empty())
            m_sendRegistered.store(false);
        else
            RegisterSend();
    }
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
        std::wcout << "Handle Error : " << errorCode << std::endl;
        break;
    }
}

int32 Session::OnRecv(BYTE* buffer, int32 len)
{
    int32 processLen = 0;

    while (true)
    {
        int32 dataSize = len - processLen;
        // 헤더 파싱
        if (dataSize < sizeof(PacketHeader))
            break;

        PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
        // 헤더에 기록된 패킷 크기 파싱
        if (dataSize < header.size)
            break;

        // 패킷  조립 성공
        OnRecvPacket(&buffer[processLen], header.size);

        processLen += header.size;
    }

    return processLen;

}

void Session::OnRecvPacket(BYTE* buffer, int32 len)
{
    SessionRef session = std::static_pointer_cast<Session>(shared_from_this());
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

    if (m_serviceType == ServiceType::CLIENT) {
        //ClientPacketHandler::HandlePacket(session, buffer, len);
    }
    else if (m_serviceType == ServiceType::SERVER) {
        ServerPacketHandler::HandlePacket(session, buffer, len);
    }


}

