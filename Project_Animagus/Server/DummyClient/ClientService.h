#pragma once


class ClientService : public std::enable_shared_from_this<ClientService>
{
public:
    ClientService(int client_count);
    ~ClientService();

    bool            Initialize();  // 네트워크 및 게임 관련 초기화
    bool            Start();
    void            ShutDown();

public:
    IocpCoreRef&    GetIocpCore() { return m_iocpCore; }
    void            AddSession(SessionRef session) { m_sessions.emplace_back(session); }

private:
    std::vector<std::thread>    m_thread;
    std::mutex                  m_mutex;

    IocpCoreRef     m_iocpCore;         // IOCP 관련 기능 담당
    std::vector<SessionRef> m_sessions;

    uint16 CLIENT_COUNT = 1;
};

