#pragma once
#include "CoreMinimal.h"
#include "../Project_Animagus.h"


class PROJECT_ANIMAGUS_API Session : public TSharedFromThis<Session>
{
public:
    Session(class FSocket* Socket);
    ~Session();

    void Run();

    UFUNCTION(BlueprintCallable)
    void HandleRecvPackets();

    void SendPacket(SendBufferRef SendBuffer);

    void Disconnect();

public:
    class FSocket* Socket;

    TSharedPtr<class RecvWorker> RecvWorkerThread;
    TSharedPtr<class SendWorker> SendWorkerThread;

    // GameThread와 NetworkThread가 데이터 주고 받는 공용 큐.
    TQueue<TArray<uint8>> RecvPacketQueue;
    TQueue<SendBufferRef> SendPacketQueue;
};


