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

    std::shared_ptr<GameServer>	GetService() { return m_server.lock(); }
    void				SetService(std::shared_ptr<GameServer> server) { m_server = server; }

public:
    // 정보 관련 //
    SOCKET				GetSocket() { return m_socket; }
    void                SetNetAddress(SOCKADDR_IN sockAddress) { m_sockAddress = sockAddress; }
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
    void                ProcessRecv(int32 numOfBytes);
    void                ProcessSend(int32 numOfBytes);

    void				HandleError(int32 errorCode);

protected:
    // 컨텐츠 코드에서 오버로딩 //
    virtual void		OnConnected() {}
    virtual int32		OnRecv(BYTE* buffer, int32 len) { return len; }
    virtual void		OnSend(int32 len) {}
    virtual void		OnDisconnected() {}


private:
    std::mutex          m_mutex;

    SOCKET              m_socket;               // 클라이언트 소켓
    SOCKADDR_IN         m_sockAddress;          // 접속 주소
    std::atomic<bool>   m_connected;            // 접속 여부
    std::weak_ptr<GameServer>   m_server;       // 이 Session이 연결된 서버


    // 버퍼 관리 관련 멤버: 내부 데이터 버퍼와 WSABUF
    BYTE*               m_buffer;
    WSABUF              m_wsaBuf;

private:
    /* IocpEvent 재사용 */
    ConnectEvent		_connectEvent;
    DisconnectEvent		_disconnectEvent;
    RecvEvent			_recvEvent;
    SendEvent			_sendEvent;
};
