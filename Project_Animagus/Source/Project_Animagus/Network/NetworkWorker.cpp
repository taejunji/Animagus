#include "NetworkWorker.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "Session.h"


/*  RecvWorker  */

RecvWorker::RecvWorker(FSocket* Socket, TSharedPtr<class Session> Session) : Socket(Socket), SessionRef(Session)
{
    Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

RecvWorker::~RecvWorker()
{
}

bool RecvWorker::Init()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));
    return true;
}

uint32 RecvWorker::Run()
{
    while (Running)
    {
        TArray<uint8> Packet;

        if (ReceivePacket(OUT Packet))
        {
            if (TSharedPtr<Session> Session = SessionRef.Pin())
            {
                Session->RecvPacketQueue.Enqueue(Packet);
            }
        }
    }

    return 0;
}

void RecvWorker::Exit()
{

}

void RecvWorker::Destroy()
{
    Running = false;
}

bool RecvWorker::ReceivePacket(TArray<uint8>& OutPacket)
{
    // 패킷 헤더 파싱
    const int32 HeaderSize = sizeof(FPacketHeader);
    TArray<uint8> HeaderBuffer;
    HeaderBuffer.AddZeroed(HeaderSize);

    if (ReceiveDesiredBytes(HeaderBuffer.GetData(), HeaderSize) == false)
        return false;

    // ID, Size 추출
    FPacketHeader Header;
    {
        FMemoryReader Reader(HeaderBuffer);
        Reader << Header;
        UE_LOG(LogTemp, Log, TEXT("Recv PacketID : %d, PacketSize : %d"), Header.PacketID, Header.PacketSize);
    }

    // 패킷 헤더 복사
    OutPacket = HeaderBuffer;

    // 패킷 내용 파싱
    TArray<uint8> PayloadBuffer;
    const int32 PayloadSize = Header.PacketSize - HeaderSize;
    if (PayloadSize == 0)
        return true;

    OutPacket.AddZeroed(PayloadSize);

    if (ReceiveDesiredBytes(&OutPacket[HeaderSize], PayloadSize))
        return true;

    return false;
}

bool RecvWorker::ReceiveDesiredBytes(uint8* Results, int32 Size)
{
    uint32 PendingDataSize;
    if (Socket->HasPendingData(PendingDataSize) == false || PendingDataSize <= 0)
        return false;

    int32 Offset = 0;

    while (Size > 0)
    {
        int32 NumRead = 0;
        Socket->Recv(Results + Offset, Size, OUT NumRead);
        check(NumRead <= Size);

        if (NumRead <= 0)
            return false;

        Offset += NumRead;
        Size -= NumRead;
    }

    return true;
}


/*  SendWorker  */

SendWorker::SendWorker(FSocket* Socket, TSharedPtr<Session> Session) : Socket(Socket), SessionRef(Session)
{
    Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
}

SendWorker::~SendWorker()
{

}

bool SendWorker::Init()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Send Thread Init")));

    return true;
}

uint32 SendWorker::Run()
{
    while (Running)
    {
        SendBufferRef SendBuffer;

        if (TSharedPtr<Session> Session = SessionRef.Pin())
        {
            if (Session->SendPacketQueue.Dequeue(OUT SendBuffer))
            {
                SendPacket(SendBuffer);
            }
        }

        // Sleep?
    }

    return 0;
}

void SendWorker::Exit()
{
}

bool SendWorker::SendPacket(SendBufferRef SendBuffer)
{
    if (SendDesiredBytes(SendBuffer->Buffer(), SendBuffer->WriteSize()) == false)
        return false;

    return true;
}

void SendWorker::Destroy()
{
    Running = false;
}

bool SendWorker::SendDesiredBytes(const uint8* Buffer, int32 Size)
{
    while (Size > 0)
    {
        int32 BytesSent = 0;
        if (Socket->Send(Buffer, Size, BytesSent) == false)
            return false;

        Size -= BytesSent;
        Buffer += BytesSent;
    }

    return true;
}
