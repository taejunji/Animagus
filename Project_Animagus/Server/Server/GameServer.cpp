#include "GameServer.h"

GameServer::GameServer() : m_running(false) 
{
    // 생성자: 멤버 초기화
}

GameServer::~GameServer()
{
    if (m_running.load()) 
    {
        Shutdown();
    }
}


bool GameServer::Initialize() 
{
    // 네트워크 초기화 작업들
    WSAData WSAData;
    ::WSAStartup(MAKEWORD(2, 2), &WSAData);
    m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);

    DWORD dwBytes;
    SOCKADDR_IN server_addr;
    ZeroMemory(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(m_listenSocket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    listen(m_listenSocket, SOMAXCONN);


    std::cout << "[GameServer] Network and IOCP initialization complete." << std::endl;
    return true;
}

void GameServer::Run()  // 메인 스레드도 이 함수 돌리는게 나을듯?
{
    m_running.store(true);

    unsigned int workerCount = std::thread::hardware_concurrency() - 1;
    if (workerCount == 0)
    {
        workerCount = 4; // 기본값
    }

    std::cout << "[GameServer] Running..." << std::endl;

    // 각 스레드가 IOCP 큐에서 이벤트를 처리하는 루프 실행
    for (unsigned int i = 0; i < workerCount; ++i) {
        m_workerThreads.emplace_back([this]() {
            while (m_running.load())
            {
                if (m_iocpCore->Dispatch(INFINITE))
                {
                    // TODO : error log
                }
            }
            });
    }

}

void GameServer::Shutdown() {
    m_running.store(false);
    std::cout << "[GameServer] Shutting down..." << std::endl;

    // 작업 스레드 종료 대기
    for (auto& thread : m_workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    m_workerThreads.clear();
    // IOCP 및 기타 자원 해제
    std::cout << "[GameServer] Shutdown complete." << std::endl;
}
