#pragma once
#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "../Project_Animagus.h"

class FSocket;
struct PROJECT_ANIMAGUS_API FPacketHeader
{
    FPacketHeader() : PacketSize(0), PacketID(0)
    {
    }

    FPacketHeader(uint16 PacketSize, uint16 PacketID) : PacketSize(PacketSize), PacketID(PacketID)
    {
    }

    friend FArchive& operator<<(FArchive& Ar, FPacketHeader& Header)
    {
        Ar << Header.PacketSize;
        Ar << Header.PacketID;
        return Ar;
    }

    uint16 PacketSize;
    uint16 PacketID;
};


/*  RecvWorker  */
class PROJECT_ANIMAGUS_API RecvWorker : public FRunnable
{
public:
    RecvWorker(FSocket* Socket, TSharedPtr<class Session> Session);
    ~RecvWorker();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Exit() override;

    void Destroy();

private:
    bool ReceivePacket(TArray<uint8>& OutPacket);
    bool ReceiveDesiredBytes(uint8* Results, int32 Size);

protected:
    FRunnableThread* Thread = nullptr;
    bool Running = true;
    FSocket* Socket;
    TWeakPtr<class Session> SessionRef;
};


/*  SendWorker  */
class PROJECT_ANIMAGUS_API SendWorker : public FRunnable
{
public:
    SendWorker(FSocket* Socket, TSharedPtr<class Session> Session);
    ~SendWorker();

    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Exit() override;

    bool SendPacket(SendBufferRef SendBuffer);

    void Destroy();

private:
    bool SendDesiredBytes(const uint8* Buffer, int32 Size);

protected:
    FRunnableThread* Thread = nullptr;
    bool Running = true;
    FSocket* Socket;
    TWeakPtr<class Session> SessionRef;
};



