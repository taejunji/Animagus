#include "pch.h"
#include "ClientService.h"
#include "SocketUtils.h"
#include "IOCPCore.h"
#include "Session.h"
#include "ClientPacketHandler.h"
#include "../Server/protocol.h"

std::atomic<int> GClientCount = 0;

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

        // ProcessConnect() 에서 AddSession() 을 호출하므로 따로 벡터에 추가 필요 X
    }


    for (unsigned int i = 0; i < 3; ++i)
    {
        m_thread.emplace_back([this]() {
            while (true)
            {
                m_iocpCore->Dispatch(10);
            }
            });
    }

    // Main Thread: 클라이언트가 랜덤한 동작을 수행, 서버로 전송
    while (true)
    {
        if (GClientCount >= CLIENT_COUNT)
        {
            for (auto& session : m_sessions) {
                SendRandomPacket(session);
            }
        }

        // 0.5초 ~ 1초 랜덤 시간 대기
        int delay = 500 + (std::rand() % 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
}

void ClientService::ShutDown()
{
    for (auto& thread : m_thread)
    {
        thread.join();
    }
}

void ClientService::SendRandomPacket(SessionRef session)
{
    using namespace Protocol;

    int packetChoice = std::rand() % 2;
    switch (packetChoice)
    {
    case 0:     // MOVE_PKT
    {
        CS_MOVE_PKT movePkt;

        // PID 를 제외한 나머지 정보는 의미 없음. 랜덤값 굳이 필요없음
        PlayerInfo info;
        info.player_id = session->playerID;
        info.x = 0.0f; info.y = 0.0f; info.z = 0.0f;
        info.rotation = 180.0f;
        info.player_type = PlayerType::NONE;
        info.player_state = PlayerState::MOVE_STATE_RUN;
        movePkt.player_info = info;

        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
        session->Send(sendBuffer);
        break;
    }

    case 1:     // SKILL_PKT
    {
        CS_USING_SKILL_PKT usingSkillPkt;

        usingSkillPkt.player_id = session->playerID;
        usingSkillPkt.room_id = 0;
        usingSkillPkt.s_type = SkillType::FIREBALL;
        usingSkillPkt.x = 0.0f; usingSkillPkt.y = 0.0f; usingSkillPkt.z = 0.0f;
        usingSkillPkt.pitch = 0.0f; usingSkillPkt.yaw = 180.0f; usingSkillPkt.roll = 0.0f;

        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(usingSkillPkt);
        session->Send(sendBuffer);
        break;
    }

    default:
        std::cout << "Invalid Random Value" << std::endl;
        exit(-1);
    }
}
