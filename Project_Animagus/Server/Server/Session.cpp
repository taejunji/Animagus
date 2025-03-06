#include "Session.h"

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
        closesocket(m_socket);
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
