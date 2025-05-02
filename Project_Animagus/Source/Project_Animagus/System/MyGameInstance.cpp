// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/AICharacter.h"
#include "../Character/PlayerCharacter.h"
#include "../Project_Animagus.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"
#include "../Character/NetworkCharacter.h"
#include "../GameMode/BattleGameMode.h"
#include "../Animation/CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../Server/Server/protocol.h"


UMyGameInstance::UMyGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

    // 메쉬 애셋 저장 -> ConstructorHelpers는 생성자에서만 호출해야한다.
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Monkey(TEXT("/Game/WorkFolder/Assets/Monkey/Character134_RiggedMonkey.Character134_RiggedMonkey"));
    if (SM_Monkey.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("SkeltalMonkeySucceeded"));
    }
    else UE_LOG(LogTemp, Warning, TEXT("SkeltalMonkeyFailed"));
    
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Tiger(TEXT("/Game/WorkFolder/Assets/Tiger/Character128_RiggedTiger.Character128_RiggedTiger"));
    if (SM_Tiger.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("SM_TigerSucceeded"));
    }
    else UE_LOG(LogTemp, Warning, TEXT("SM_TigerFailed"));
    
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Koala(TEXT("/Game/WorkFolder/Assets/Koala/Character139_RiggedKoala.Character139_RiggedKoala"));
    if (SM_Koala.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("SM_KoalaSucceeded"));
    }
    else UE_LOG(LogTemp, Warning, TEXT("SM_KoalaFailed"));
    
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Sheep(TEXT("/Game/WorkFolder/Assets/Sheep/Character144_RiggedSheep.Character144_RiggedSheep"));
    if (SM_Sheep.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("SM_SheepSucceeded"));
    }
    else UE_LOG(LogTemp, Warning, TEXT("SM_SheepFailed"));
    
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Fox(TEXT("/Game/WorkFolder/Assets/Fox/Character131_RiggedFox.Character131_RiggedFox"));
    if (SM_Fox.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("SM_FoxSucceeded"));
    }
    else UE_LOG(LogTemp, Warning, TEXT("SM_FoxFailed"));
    
    if (SM_Monkey.Succeeded()) CharacterMeshes.Add(TEXT("SM_Monkey"), SM_Monkey.Object);
    if (SM_Tiger.Succeeded()) CharacterMeshes.Add(TEXT("SM_Tiger"), SM_Tiger.Object);
    if (SM_Koala.Succeeded()) CharacterMeshes.Add(TEXT("SM_Koala"), SM_Koala.Object);
    if (SM_Sheep.Succeeded()) CharacterMeshes.Add(TEXT("SM_Sheep"), SM_Sheep.Object);
    if (SM_Fox.Succeeded()) CharacterMeshes.Add(TEXT("SM_Fox"), SM_Fox.Object);
}

void UMyGameInstance::Init()
{
    Super::Init();

    ConnectToGameServer();

    // 데이터 불러왔을 때 mesh 타입 설정하기 => ( 임시로 양 디폴트 )
    player_data.stored_mesh = CharacterMesh::Sheep; 

    InitGameInstance();
}

void UMyGameInstance::Shutdown()
{
    Super::Shutdown();

    DisconnectFromGameServer();
    ClientSession->Disconnect();
}

void UMyGameInstance::InitGameInstance()
{
    // 배틀 처음 시작 시 -> 라운드 0, 레벨 Battle로 설정
    round_count = 0;
    // current_level = LevelType::Battle;
}

void UMyGameInstance::SwitchLevel(LevelType level)
{
    switch (level)
    {
    case LevelType::Login:
        UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/Login"));
        break;

    case LevelType::Lobby:
        UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/Lobby"));
        break;

    case LevelType::Battle:
        UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/L_Map"));
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
            // TODO : 일단 인스턴스 시작하자마자 게임 입장 패킷 보냄. 로그인 패킷으로 변경 필요
            Protocol::CS_ENTER_GAME_PKT pkt;
            pkt.room_id = 0;
            SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
            SendPacket(sendBuffer);
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

    Protocol::CS_LEAVE_PKT LeavePkt;
    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(LeavePkt);
    Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
}

void UMyGameInstance::HandleRecvPackets()
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    ClientSession->HandleRecvPackets();
}

void UMyGameInstance::SendPacket(SendBufferRef SendBuffer)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    ClientSession->SendPacket(SendBuffer);
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


void UMyGameInstance::HandleEnterGame(Protocol::SC_ENTER_GAME_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    MyPlayerId = pkt.player_id;

    auto* World = GetWorld();
    if (World == nullptr)
        return;
    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            //GameMode->SetPlayerIndex(pkt.player_id);
            //GameMode->SetPlayerType(pkt.player_type);
            //GameMode->SetPlayerName(pkt.player_name);
            GameMode->PossessIndex = pkt.spawn_index;
            GameMode->AmIHost = pkt.host;
            if (pkt.host == true)
                UE_LOG(LogTemp, Warning, TEXT("I AM THE HOST: %d"), pkt.player_id);
                
            GameMode->SpawnPlayers();

            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC == nullptr) return;

            if (APlayerCharacter* MyPlayer = Cast<APlayerCharacter>(PC->GetPawn()))
            {
                MyPlayer->SetPlayerId(pkt.player_id);
            }
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("PlayerIndex: %d"), pkt.player_id);

}

// InstanceHandlePacket
void UMyGameInstance::HandleSpawn(Protocol::SC_SPAWN_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            GameMode->SpawnPlayer(pkt);
        }
    }
}

void UMyGameInstance::HandleMove(Protocol::CS_MOVE_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            GameMode->MoveOtherPlayer(pkt);
        }
    }
}

void UMyGameInstance::HandleSkill(Protocol::CS_USING_SKILL_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            GameMode->SpawnSkill(pkt);
        }
    }
}

void UMyGameInstance::HandleSpawnItem(Protocol::SC_SPAWN_ITEM_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            GameMode->SpawnItem(pkt);
        }
    }
}

void UMyGameInstance::HandleUpdateHp(Protocol::SC_UPDATE_HP_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            GameMode->UpdateHp(pkt);
        }
    }
}



//void UMyGameInstance::SetMyPlayerIndex(uint16 playerIndex)
//{
//    MyPlayerIndex = playerIndex;
//
//    auto* World = GetWorld();
//    if (World == nullptr)
//        return;
//
//    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
//    if (BaseGameMode)
//    {
//        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
//        if (GameMode)
//        {
//            GameMode->SetPlayerIndex(MyPlayerIndex);
//        }
//    }
//}
