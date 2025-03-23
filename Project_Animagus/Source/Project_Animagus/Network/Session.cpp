#include "Session.h"
#include "Sockets.h"

#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"	
#include "SocketSubsystem.h"

#include "ClientPacketHandler.h"
#include "NetworkWorker.h"


Session::Session(class FSocket* Socket) : Socket(Socket)
{
    ClientPacketHandler::Init();
}

Session::~Session()
{
    Disconnect();
}

void Session::Run()
{
    // 2개의 네트워크 작업 스레드 생성(활성화)
    RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
    SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
}

void Session::HandleRecvPackets()
{
    // 조립완료된 패킷들 큐에서 패킷 꺼내서 처리 반복
    while (true)
    {
        //UE_LOG(LogTemp, Log, TEXT("Handle Recv"));

        TArray<uint8> Packet;
        if (RecvPacketQueue.Dequeue(OUT Packet) == false)
            break;

        // 패킷 처리
        SessionRef ThisPtr = AsShared();
        ClientPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());
    }
}

void Session::SendPacket(SendBufferRef SendBuffer)
{
    SendPacketQueue.Enqueue(SendBuffer);
}

void Session::Disconnect()
{
    if (RecvWorkerThread)
    {
        RecvWorkerThread->Destroy();
        RecvWorkerThread = nullptr;
    }

    if (SendWorkerThread)
    {
        SendWorkerThread->Destroy();
        SendWorkerThread = nullptr;
    }

    if (Socket)
        Socket->Close();
}
