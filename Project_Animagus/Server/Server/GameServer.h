#pragma once

class GameServer : public std::enable_shared_from_this<GameServer>
{
public:
    GameServer();
    ~GameServer();

    bool            Initialize();  // 네트워크 및 게임 관련 초기화
    void            Run();         // 서버 메인 루프
    void            Shutdown();    // 서버 종료 및 자원 해제


    // TODO : 클라이언트 패킷 관리 시스템 추가
    void			AddSession(SessionRef session);
    void			ReleaseSession(SessionRef session);

public:
    IocpCoreRef&    GetIocpCore() { return m_iocpCore; }
    int32			GetCurrentSessionCount() { return m_sessionCount; }
    int32			GetMaxSessionCount() { return m_maxSessionCount; }

private:
    std::mutex                  m_mutex;

    IocpCoreRef                 m_iocpCore;         // IOCP 관련 기능 담당
    std::vector<std::thread>    m_workerThreads;    // IOCP 작업 스레드 풀
    std::atomic<bool>           m_running;          // 서버 실행 상태 플래그

    ListenerRef                 m_listener;
    std::set<SessionRef>	    m_sessions;
    int32			            m_sessionCount = 0;
    int32			            m_maxSessionCount = 0;

    // TODO : 게임 로직 및 클라이언트 관리 관련 멤버 추가
};

