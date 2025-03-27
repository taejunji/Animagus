#include "pch.h"
#include "ClientService.h"
#include "SocketUtils.h"
#include "IOCPCore.h"
#include "Session.h"
#include "ClientPacketHandler.h"


ClientService::ClientService(int client_count) : CLIENT_COUNT(client_count)
{
}

ClientService::~ClientService()
{
}

bool ClientService::Initialize()
{
    WSADATA wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return false;
    }
    SocketUtils::Init();
    ClientPacketHandler::Init();

    m_iocpCore = std::make_shared<IocpCore>();
    if (!m_iocpCore || m_iocpCore->GetHandle() == nullptr)
    {
        std::cerr << "Failed to create IOCP handle." << std::endl;
        return false;
    }

    m_sessions.reserve(CLIENT_COUNT);

    return true;
}

bool ClientService::Start()
{
    for (int i = 0; i < CLIENT_COUNT; ++i)
    {
        SessionRef session = std::make_shared<Session>(Session::ServiceType::CLIENT);
        if (session == nullptr)
            return false;

        session->SetService(shared_from_this());

        if (false == m_iocpCore->Register(session))
            return false;

        if (session->Connect() == false) {
            std::cout << "Connect Fail" << std::endl;
            return false;
        }
    }
    std::cout << "Connect " << CLIENT_COUNT << std::endl;


    for (unsigned int i = 0; i < 2; ++i)
    {
        m_thread.emplace_back([this]() {
            while (true)
            {
                m_iocpCore->Dispatch(10);
            }
            });
    }


    while (true)
    {
        m_iocpCore->Dispatch(10);
    }
}

void ClientService::ShutDown()
{
    for (auto& thread : m_thread)
    {
        thread.join();
    }
}
