#include "pch.h"
#include "GameServer.h"
#include "IocpCore.h"
#include "Session.h"
#include "Listener.h"
#include "SocketUtils.h"


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
    WSADATA wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return false;
    }
    SocketUtils::Init();

    m_iocpCore = std::make_shared<IocpCore>();
    if (!m_iocpCore || m_iocpCore->GetHandle() == nullptr)
    {
        std::cerr << "Failed to create IOCP handle." << std::endl;
        return false;
    }

    //// 리슨 소켓 생성
    //m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
    //if (m_listenSocket == INVALID_SOCKET) {
    //    std::cerr << "Failed to create listen socket." << std::endl;
    //    return false;
    //}

    //SOCKADDR_IN serverAddr;
    //ZeroMemory(&serverAddr, sizeof(serverAddr));
    //serverAddr.sin_family = AF_INET;
    //serverAddr.sin_port = htons(SERVER_PORT);           // 포트 번호
    //serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 모든 인터페이스
    //if (bind(m_listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
    //    std::cerr << "Bind failed." << std::endl;
    //    closesocket(m_listenSocket);
    //    return false;
    //}
    //if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
    //    std::cerr << "Listen failed." << std::endl;
    //    closesocket(m_listenSocket);
    //    return false;
    //}

    //// 리슨소켓 CP등록
    //if (m_iocpCore->Register(&m_listener))
    //{
    //    std::cerr << "리슨 소켓 IOCP 등록 실패." << std::endl;
    //    return false;
    //}

    //HANDLE iocpHandle = m_iocpCore->GetHandle();
    //if (CreateIoCompletionPort(reinterpret_cast<HANDLE>(m_listenSocket), iocpHandle, 0, 0) == nullptr) {
    //    std::cerr << "리슨 소켓 IOCP 등록 실패." << std::endl;
    //    return false;
    //}

    // TODO : AcceptEx 등록
    m_listener = std::make_shared<Listener>();
    if (m_listener == nullptr)
        return false;

    GameServerRef server = std::static_pointer_cast<GameServer>(shared_from_this());
    if (m_listener->StartAccept(server) == false)
        return false;


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
    m_workerThreads.resize(workerCount);

    std::cout << "[GameServer] Running..." << std::endl;

    // 각 스레드가 IOCP 큐에서 이벤트를 처리하는 루프 실행
    for (unsigned int i = 0; i < workerCount; ++i)
    {
        m_workerThreads.emplace_back([this]() {
            while (m_running.load())
            {
                if (m_iocpCore->Dispatch(10))
                {
                    // TODO : error log
                }
            }
            });
    }

    // 메인 스레드
    while (m_running.load())
    {
        if (m_iocpCore->Dispatch(10))
        {
            // TODO : error log
        }
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

    // TODO : IOCP 및 기타 자원 해제
    m_workerThreads.clear();
    m_listener->CloseSocket();



    std::cout << "[GameServer] Shutdown complete." << std::endl;
}

void GameServer::AddSession(SessionRef session)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_sessionCount++;
    m_sessions.insert(session);
}

void GameServer::ReleaseSession(SessionRef session)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_sessions.erase(session) != 0);
    m_sessionCount--;
}
