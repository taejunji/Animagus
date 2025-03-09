#pragma once


class Session : public IocpObject
{
    friend class Listener;
    friend class IocpCore;

public:
    static constexpr size_t BUFFER_SIZE = 0x1'0000; // 64KB

public:
    Session();
    virtual ~Session();

    // IocpObject 인터페이스 구현
    virtual HANDLE GetHandle() override;
    virtual void Dispatch(IocpEvent* iocpEvent, int32_t numOfBytes = 0) override;

public:
    // 외부에서 사용 //
    void				Send(BYTE* buffer, int32 len);
    bool				Connect();
    void				Disconnect(const WCHAR* cause);

public:
    // 정보 관련 //
    SOCKET				GetSocket() { return m_socket; }
    bool				IsConnected() { return m_connected; }
    SessionRef			GetSessionRef() { return std::static_pointer_cast<Session>(shared_from_this()); }

private:
    // 전송 관련 //
    bool				RegisterConnect();
    bool				RegisterDisconnect();
    void				RegisterRecv();
    void				RegisterSend();

    void				ProcessConnect();
    void				ProcessDisconnect();
    void                ProcessRecv(int32_t numOfBytes);
    void                ProcessSend(int32_t numOfBytes);
    void                ProcessAccept(int32_t numOfBytes);

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
    bool StartAccept(IocpCoreRef iocpCore);
    void CloseSocket();

public:
    /* 인터페이스 구현 */
    virtual HANDLE GetHandle() override;
    virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
    /* 수신 관련 */
    void RegisterAccept(AcceptEvent* acceptEvent);
    void ProcessAccept(AcceptEvent* acceptEvent);

protected:
    SOCKET _socket = INVALID_SOCKET;
    std::vector<AcceptEvent*> _acceptEvents;

    IocpCoreRef _iocpCore;      // Listener 가 멤버로 있는 Server 의 IocpCore
};

