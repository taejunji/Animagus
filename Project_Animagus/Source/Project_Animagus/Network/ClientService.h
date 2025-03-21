#pragma once


class ClientService : public std::enable_shared_from_this<ClientService>
{
public:
    ClientService();
    ~ClientService();

    bool            Initialize();  // 네트워크 및 게임 관련 초기화
    bool            Start();

public:
    IocpCoreRef&    GetIocpCore() { return m_iocpCore; }
    void            SetSession(SessionRef session) { m_session = session; }

private:

    IocpCoreRef     m_iocpCore;         // IOCP 관련 기능 담당
    SessionRef      m_session;
};

