// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/AICharacter.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "../Network/Session.h"


UMyGameInstance::UMyGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("게임 인스턴스 생성자"));
    }

    // 메쉬 애셋 저장 -> ConstructorHelpers는 생성자에서만 호출해야한다.
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Monkey(TEXT("/Game/WorkFolder/Assets/Monkey/Character134_RiggedMonkey.Character134_RiggedMonkey"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Tiger(TEXT("/Game/WorkFolder/Assets/Tiger/Character128_RiggedTiger.Character128_RiggedTiger"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Koala(TEXT("/Game/WorkFolder/Assets/Koala/Character139_RiggedKoala.Character139_RiggedKoala"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Sheep(TEXT("/Game/WorkFolder/Assets/Sheep/Character144_RiggedSheep.Character144_RiggedSheep"));

    if (SM_Monkey.Succeeded()) CharacterMeshes.Add(TEXT("SM_Monkey"), SM_Monkey.Object);
    if (SM_Tiger.Succeeded()) CharacterMeshes.Add(TEXT("SM_Tiger"), SM_Tiger.Object);
    if (SM_Koala.Succeeded()) CharacterMeshes.Add(TEXT("SM_Koala"), SM_Koala.Object);
    if (SM_Sheep.Succeeded()) CharacterMeshes.Add(TEXT("SM_Sheep"), SM_Sheep.Object);
}

void UMyGameInstance::Init()
{
    Super::Init();

    // 데이터 불러왔을 때 mesh 타입 설정하기 => ( 임시로 양 디폴트 )
    player_data.stored_mesh = CharacterMesh::Sheep; 

    InitGameInstance();
}

void UMyGameInstance::Shutdown()
{
    Super::Shutdown();
}

void UMyGameInstance::InitGameInstance()
{
    // 배틀 처음 시작 시 -> 라운드 0, 레벨 Battle로 설정
    round_count = 0;
    current_level = LevelType::Battle;
}

void UMyGameInstance::SwitchLevel(LevelType level)
{
    switch (level)
    {
    case LevelType::Login:
        UGameplayStatics::OpenLevel(GetWorld(), FName("Login"));
        break;

    case LevelType::Lobby:
        UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"));
        break;

    case LevelType::Battle:
        UGameplayStatics::OpenLevel(GetWorld(), FName("L_Map"));
        break;
    }
}

void UMyGameInstance::ConnectToGameServer()
{
    // TCP 소켓 생성
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

    FIPv4Address Ip;
    FIPv4Address::Parse(IpAddress, Ip);

    TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    InternetAddr->SetIp(Ip.Value);
    InternetAddr->SetPort(Port);

    // Log
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));

    bool Connected = Socket->Connect(*InternetAddr);	// Blocking 방식 Connect

    if (Connected)
    {
        // Log
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));

        // Session
        ClientSession = MakeShared<Session>(Socket);
        ClientSession->Run();

        {


        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
    }
}

void UMyGameInstance::DisconnectFromGameServer()
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    Protocol::C_LEAVE_GAME LeavePkt;
    SEND_PACKET(LeavePkt);
}

//void UMyGameInstance::AddAICharacter(AAICharacter* AICharacter)
//{
//    if (AICharacter)
//    {
//        AIPlayers.Add(AICharacter);
//    }
//}

void UMyGameInstance::PrintGameInstanceData()
{
    FString DebugMessage = FString::Printf(TEXT("게임 인스턴스 - 배틀 라운드 수: %d"), round_count);
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, DebugMessage);
}
