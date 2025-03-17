#include "pch.h"
#include "ClientService.h"
#include "SocketUtils.h"
#include "IOCPCore.h"
#include "Session.h"


ClientService::ClientService()
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

    m_iocpCore = std::make_shared<IocpCore>();
    if (!m_iocpCore || m_iocpCore->GetHandle() == nullptr)
    {
        std::cerr << "Failed to create IOCP handle." << std::endl;
        return false;
    }

    m_session = std::make_shared<Session>(Session::ServiceType::CLIENT);
    if (m_session == nullptr)
        return false;
    m_session->SetService(shared_from_this());

    m_iocpCore->Register(m_session);

}

bool ClientService::Start()
{
    if (m_session->Connect() == false)
        return false;

    while (true)
    {
        if (false == m_iocpCore->Dispatch(10))
        {
            return false;
        }
    }


}
