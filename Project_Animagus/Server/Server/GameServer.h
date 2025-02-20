#pragma once
#include "pch.h"

class GameServer
{
public:
    GameServer();
    ~GameServer();

    bool Initialize();  // 네트워크 및 게임 관련 초기화
    void Run();         // 서버 메인 루프
    void Shutdown();    // 서버 종료 및 자원 해제

    // TODO : 클라이언트 패킷 관리 시스템 추가

private:
    SOCKET                      m_listenSocket;
    IocpCore*                   m_iocpCore;         // IOCP 관련 기능 담당
    std::vector<std::thread>    m_workerThreads;    // IOCP 작업 스레드 풀
    std::atomic<bool>           m_running;          // 서버 실행 상태 플래그

    // TODO : 게임 로직 및 클라이언트 관리 관련 멤버 추가
};

