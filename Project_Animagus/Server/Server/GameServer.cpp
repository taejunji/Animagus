#include "pch.h"
#include "GameServer.h"
#include "IocpCore.h"
#include "Session.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "ServerPacketHandler.h"


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
    ServerPacketHandler::Init();

    m_iocpCore = std::make_shared<IocpCore>();
    if (!m_iocpCore || m_iocpCore->GetHandle() == nullptr)
    {
        std::cerr << "Failed to create IOCP handle." << std::endl;
        return false;
    }

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
                m_iocpCore->Dispatch(10);
            }
            });
    }

    // 메인 스레드
    while (m_running.load())
    {
        //m_iocpCore->Dispatch(10);

        if (m_sessionCount == 0) continue;
        DCS_TEST_PKT pkt;
        std::string msg = "Hello";
        ::memcpy(pkt.msg, msg.c_str(), msg.size());
        pkt.len = 5;

        SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
        for (auto& session : m_sessions)
            session->Send(sendBuffer);

        std::this_thread::sleep_for(std::chrono::seconds(1));
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
