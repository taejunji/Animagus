#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"


class Session : public IocpObject
{
public:
    static constexpr size_t BUFFER_SIZE = 0x1'0000; // 64KB

public:
    Session();
    virtual ~Session();

    // IocpObject 인터페이스 구현
    virtual HANDLE GetHandle() override;
    virtual void Dispatch(IocpEvent* iocpEvent, int32_t numOfBytes = 0) override;

    // 각 이벤트 타입에 따른 처리 함수
    void ProcessRecv(int32_t numOfBytes);
    void ProcessSend(int32_t numOfBytes);
    void ProcessAccept(int32_t numOfBytes);

public:
    SOCKET              m_socket;      // 클라이언트 소켓
    //NetAddress        m_netAddress;  // 접속 주소
    std::atomic<bool>   m_connected;  // 접속 여부

    // 버퍼 관리 관련 멤버: 내부 데이터 버퍼와 WSABUF
    char*               m_buffer;
    WSABUF              m_wsaBuf;
};


/*--------------
    Listener
---------------*/

class Listener : public IocpObject
{
public:
    Listener() = default;
    ~Listener();

public:
    /* 외부에서 사용 */
    bool StartAccept();
    void CloseSocket();

    //bool Bind();
    bool BindAnyAddess();
    bool Listen(int32 backlog = SOMAXCONN);

public:
    /* 인터페이스 구현 */
    virtual HANDLE GetHandle() override;
    virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
    /* 수신 관련 */
    void RegisterAccept(IocpEvent* acceptEvent);
    void ProcessAccept(IocpEvent* acceptEvent);

protected:
    SOCKET _socket = INVALID_SOCKET;
    std::vector<IocpEvent*> _acceptEvents;
};

