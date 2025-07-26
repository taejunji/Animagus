// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/AICharacter.h"
#include "../Character/PlayerCharacter.h"
#include "../Project_Animagus.h"
#include "GameFramework/PlayerController.h"

#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"
#include "../Character/NetworkCharacter.h"
#include "../GameMode/BattleGameMode.h"
#include "../GameMode/LoginGameMode.h"
#include "../GameMode/ConnectGameMode.h"
#include "../Animation/CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Actor/ItemBox/Item_Box_Base.h"
#include "../Actor/Zones/ShrinkingZone.h"
#include "../Actor/Zones/AttractionZone.h"
#include "../PlayerController/ConnectPlayerController.h"
#include "../PlayerController/Battle_PlayerController.h"
#include "../UI/RoundResultWidget.h"
#include "../UI/MyPlayerHUDWidget.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "../Server/Server/protocol.h"

#include "Components/AudioComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "TimerManager.h"
//#include "Misc/DisplayMetrics.h"


UMyGameInstance::UMyGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

    // 메쉬 애셋 저장 -> ConstructorHelpers는 생성자에서만 호출해야한다.
    // static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Tiger(TEXT("/Game/WorkFolder/Assets/Tiger/Character128_RiggedTiger.Character128_RiggedTiger"));
    // if (SM_Tiger.Succeeded()) CharacterMeshes.Add(TEXT("SM_Tiger"), SM_Tiger.Object);

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Monkey(TEXT("/Game/WorkFolder/Assets/Monkey/Character134_RiggedMonkey.Character134_RiggedMonkey"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Koala(TEXT("/Game/WorkFolder/Assets/Koala/Character139_RiggedKoala.Character139_RiggedKoala"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Sheep(TEXT("/Game/WorkFolder/Assets/Sheep/Character144_RiggedSheep.Character144_RiggedSheep"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Fox(TEXT("/Game/WorkFolder/Assets/Fox/Character131_RiggedFox.Character131_RiggedFox"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Sloth(TEXT("/Game/WorkFolder/Assets/Sloth/Character149_RiggedSloth.Character149_RiggedSloth")); 
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Elephant(TEXT("/Game/WorkFolder/Assets/Elephant/Character136_RiggedElephant.Character136_RiggedElephant"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Raccoon(TEXT("/Game/WorkFolder/Assets/Raccoon/Character133_RiggedRaccoon.Character133_RiggedRaccoon"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Deer(TEXT("/Game/WorkFolder/Assets/Deer/Character146_RiggedDeer.Character146_RiggedDeer"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Cow(TEXT("/Game/WorkFolder/Assets/Cow/Character132_RiggedCow.Character132_RiggedCow")); 
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Unicorn(TEXT("/Game/WorkFolder/Assets/Unicorn/Character143_RiggedUnicorn.Character143_RiggedUnicorn")); 
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Zebra(TEXT("/Game/WorkFolder/Assets/Zebra/Character142_RiggedZebra.Character142_RiggedZebra")); 
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Donkey(TEXT("/Game/WorkFolder/Assets/Donkey/Character148_RiggedDonkey.Character148_RiggedDonkey")); 

    if (SM_Monkey.Succeeded()) CharacterMeshes.Add(TEXT("SM_Monkey"), SM_Monkey.Object);
    if (SM_Koala.Succeeded()) CharacterMeshes.Add(TEXT("SM_Koala"), SM_Koala.Object);
    if (SM_Sheep.Succeeded()) CharacterMeshes.Add(TEXT("SM_Sheep"), SM_Sheep.Object);
    if (SM_Fox.Succeeded()) CharacterMeshes.Add(TEXT("SM_Fox"), SM_Fox.Object);

    if (SM_Sloth.Succeeded()) CharacterMeshes.Add(TEXT("SM_Sloth"), SM_Sloth.Object); 
    if (SM_Elephant.Succeeded()) CharacterMeshes.Add(TEXT("SM_Elephant"), SM_Elephant.Object); 
    if (SM_Raccoon.Succeeded()) CharacterMeshes.Add(TEXT("SM_Raccoon"), SM_Raccoon.Object); 
    if (SM_Deer.Succeeded()) CharacterMeshes.Add(TEXT("SM_Deer"), SM_Deer.Object);

    if (SM_Cow.Succeeded()) CharacterMeshes.Add(TEXT("SM_Cow"), SM_Cow.Object);
    if (SM_Unicorn.Succeeded()) CharacterMeshes.Add(TEXT("SM_Unicorn"), SM_Unicorn.Object);
    if (SM_Zebra.Succeeded()) CharacterMeshes.Add(TEXT("SM_Zebra"), SM_Zebra.Object); 
    if (SM_Donkey.Succeeded()) CharacterMeshes.Add(TEXT("SM_Donkey"), SM_Donkey.Object);

}

void UMyGameInstance::Init()
{
    Super::Init();

    UGameUserSettings* Settings = GEngine->GetGameUserSettings();
    if (Settings)
    {
        FDisplayMetrics DisplayMetrics;
        FDisplayMetrics::RebuildDisplayMetrics(DisplayMetrics);

        FIntPoint MaxResolution = FIntPoint(
            DisplayMetrics.PrimaryDisplayWidth,
            DisplayMetrics.PrimaryDisplayHeight
        );

        //FVector2D resolution;
        //resolution.X = GSystemResolution.ResX;
        //resolution.Y = GSystemResolution.ResY;

        Settings->SetFullscreenMode(EWindowMode::Fullscreen);
        //Settings->SetScreenResolution(FIntPoint(resolution.X, resolution.Y));
        Settings->SetScreenResolution(MaxResolution);
        Settings->ApplySettings(false);
    }

    ConnectToGameServer();

    // 데이터 불러왔을 때 mesh 타입 설정하기 => ( 임시로 양 디폴트 )
    player_data.stored_mesh = CharacterMesh::Sheep;

    InitGameInstance();
}

void UMyGameInstance::Shutdown()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearAllTimersForObject(this);
    }

    DisconnectFromGameServer();
    ClientSession->Disconnect();

    Super::Shutdown();
}

void UMyGameInstance::InitGameInstance()
{
    // 배틀 처음 시작 시 -> 라운드 0, 레벨 Battle로 설정
    // 0: 아침
    // 1: 저녁
    // 2: 밤
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
#if WITH_EDITOR
    // 에디터: 프로젝트 폴더에
    FString IpFilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("ip.txt"));
#else
    // 패키징: exe 폴더에
    FString IpFilePath = FPaths::Combine(FPaths::LaunchDir(), TEXT("ip.txt"));
#endif

    FString IpAddress;
    if (false == FFileHelper::LoadFileToString(IpAddress, *IpFilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load IP Addr file: %s"), *IpFilePath);
    }
    // 앞뒤 공백 제거
    IpAddress = IpAddress.TrimStartAndEnd();
    UE_LOG(LogTemp, Warning, TEXT("IP Address: %s"), *IpAddress);

    // TCP 소켓 생성
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

    FIPv4Address Ip;
    FIPv4Address::Parse(IpAddress, Ip);

    TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    InternetAddr->SetIp(Ip.Value);
    InternetAddr->SetPort(Port);

    // Log
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connecting To Server...")));
    UE_LOG(LogTemp, Warning, TEXT("Connecting To Server..."));


    bool Connected = Socket->Connect(*InternetAddr);	// Blocking 방식 Connect

    if (Connected)
    {
        // Log
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Success")));
        UE_LOG(LogTemp, Warning, TEXT("Connection Success"));

        // Session
        ClientSession = MakeShared<Session>(Socket);
        ClientSession->Run();

        {
            // TODO : 일단 인스턴스 시작하자마자 룸 입장 패킷 보냄. 로그인 패킷으로 변경 필요
            //Protocol::CS_ENTER_ROOM_PKT pkt;
            //pkt.room_id = 0;
            //SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
            //SendPacket(sendBuffer);
        }
    }
    else
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connection Failed")));
        UE_LOG(LogTemp, Warning, TEXT("Connection Failed"));
    }
}

void UMyGameInstance::DisconnectFromGameServer()
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    // TODO: 게임 종료 패킷 보내도록 수정
    //Protocol::CS_LEAVE_PKT LeavePkt;
    //SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(LeavePkt);
    //Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
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

void UMyGameInstance::SaveBGMPlaybackTime()
{
    if (MenuBGMComponent && MenuBGMComponent->IsPlaying())
    {
        //MenuBGMPlaybackTime = MenuBGMComponent->GetPlaybackTime();
    }
}

void UMyGameInstance::PauseMenuBGM()
{
    if (MenuBGMComponent && MenuBGMComponent->IsPlaying())
    {
        MenuBGMComponent->SetPaused(true);
    }
}

void UMyGameInstance::ResetMenuBGM()
{
    if (MenuBGMComponent && MenuBGMComponent->IsValidLowLevel())
    {
        MenuBGMComponent->Stop();
        MenuBGMComponent->DestroyComponent();
    }

    // 메뉴 레벨로 돌아올 때마다 새로 생성 & Looping
    MenuBGMComponent = UGameplayStatics::SpawnSound2D(
        GetWorld(),       // 반드시 현재 월드 컨텍스트
        MenuBGM,          // SoundBase*
        1.f, 1.f, 0.f,    // 볼륨, 피치, 스타트타임
        nullptr,          // Attenuation
        true // 반복 재생
    );

}

void UMyGameInstance::OnStart()
{
    Super::OnStart();

    if (MenuBGM && !MenuBGMComponent)
    {
        // 이제는 GetWorld()가 유효하고 AudioDevice도 준비된 후
        MenuBGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), MenuBGM, 1.f, 1.f, 0.f, nullptr, true);
        // 마지막 파라미터(true)로 Looping까지 한 번에 걸 수 있습니다.
    }

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
    //GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, DebugMessage);
    UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMessage);
}


void UMyGameInstance::HandleLobbyHost(Protocol::SC_UR_HOST_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    AmIHost = true;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        if (AConnectGameMode* GameMode = Cast<AConnectGameMode>(BaseGameMode))
        {
            if (AConnectPlayerController* PC = Cast<AConnectPlayerController>(UGameplayStatics::GetPlayerController(GameMode, 0))) {
                GameMode->AmIHost = true;
                PC->ActiveStartButton();
            }
        }
    }
}

void UMyGameInstance::HandleStartGame(Protocol::SC_START_GAME_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;
    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        AConnectGameMode* GameMode = Cast<AConnectGameMode>(BaseGameMode);
        if (GameMode == nullptr) return;
    }

    static const FName TargetLevelName = TEXT("L_Map");
    UE_LOG(LogTemp, Log, TEXT("레벨 '%s' 로 전환합니다."), *TargetLevelName.ToString());

    // OpenLevel 호출:
    UGameplayStatics::OpenLevel(this, TargetLevelName);
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
            GameMode->StartTime2Server = pkt.server_time;
            if (pkt.host == true)
            {
                //GameMode->CurrentPlayerCount = pkt.player_count;
                UE_LOG(LogTemp, Warning, TEXT("I AM THE HOST: %d"), pkt.player_id);
            }
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

void UMyGameInstance::HandleBattleRoundEnd(Protocol::SC_ROUND_END_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    UE_LOG(LogTemp, Warning, TEXT("라운드 끝!"));

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            //GameMode->InactiveInput();
            TArray<FString> Names;
            TArray<int32>   Scores;

            for (int8 i = 0; i < 8; ++i)
            {
                int32 IdValue = static_cast<int32>(pkt.ranking[i]);
                if (MyPlayerId == IdValue) myRank = i + 1;

                //FString PlayerId = FString::FromInt(IdValue);
                //Names.Add(PlayerId);    // 이름 추가하기 전까지는 Id 로

                const TCHAR* Name = UTF8_TO_TCHAR(pkt.name[i]);
                Names.Add(FString(Name));
                Scores.Add(static_cast<int32>(pkt.score[i]));
            }

            GameMode->OnRoundEnd(Names, Scores);
        }
    }
}

void UMyGameInstance::HandleBattleRoundInit(Protocol::SC_ROUND_INIT_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    UE_LOG(LogTemp, Warning, TEXT("배틀모드 초기화"));

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            if (ABattle_PlayerController* PC = Cast<ABattle_PlayerController>(
                UGameplayStatics::GetPlayerController(this, 0))) {
                PC->RoundResultwidget->HideSelf();
                PC->PlayerHUD->ResetLevelImgage();
            }
            round_count++;
            GameMode->isRoundEnd = false;
            GameMode->ShrinkingZone->Destroy();

            for (auto& Item : GameMode->AttractionZones)
                if (Item != nullptr) Item->Destroy();
            for (auto& Item : GameMode->SpawnedItemBoxes)
                if (Item != nullptr) Item->Destroy();
            for (auto& Item : GameMode->SpawnedItems)
                if (Item != nullptr) Item->Destroy();
            for (auto& Item : GameMode->SpawnedPlayers)
            {
                UE_LOG(LogTemp, Warning, TEXT("Player ID: %d"), Item.Key);
                if (Item.Value != nullptr) Item.Value->Destroy();
            }
            //for (auto& Item : GameMode->IndexingSpawnedPlayers)
            //    if (Item != nullptr) Item->Destroy();

            GameMode->InitBattleMode();
            GameMode->PlayBackgroundMusic();
        }
    }
}

void UMyGameInstance::HandleAISpawn(Protocol::SC_AI_SPAWN_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    if (false == AmIHost) return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode);
        if (GameMode)
        {
            GameMode->SpawnAIPlayers(pkt);
        }
    }
}

void UMyGameInstance::HandleJumpEffect(Protocol::CS_JUMP_EFT_PKT& pkt)
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
            GameMode->HandleJumpEffect(pkt);
        }
    }
}

void UMyGameInstance::HandleBattleModeEnd(Protocol::SC_GAME_END_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    round_count = 0;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        if (ABattleGameMode* GameMode = Cast<ABattleGameMode>(BaseGameMode)) 
        {
            UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/L_Result"));
        }
    }
}

void UMyGameInstance::HandleLoginSuccess(Protocol::SC_LOGIN_SUCC_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        if (ALoginGameMode* GameMode = Cast<ALoginGameMode>(BaseGameMode))
        {
            UE_LOG(LogTemp, Warning, TEXT("로그인 성공"));

            MyPlayerName = FString(UTF8_TO_TCHAR(pkt.player_name));
            GameMode->HandleLoginSuccess();
            //UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/L_MainMenu"));
        }
    }
}

void UMyGameInstance::HandleLoginFail(Protocol::SC_LOGIN_FAIL_PKT& pkt)
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        if (ALoginGameMode* GameMode = Cast<ALoginGameMode>(BaseGameMode))
        {
            UE_LOG(LogTemp, Warning, TEXT("로그인 실패"));

            GameMode->HandleLoginFail(pkt);
        }
    }
}

void UMyGameInstance::HandleSignUpSuccess()
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        if (ALoginGameMode* GameMode = Cast<ALoginGameMode>(BaseGameMode))
        {
            GameMode->HandleSignUpSuccess();
        }
    }
}

void UMyGameInstance::HandleSignUpFail()
{
    if (Socket == nullptr || ClientSession == nullptr)
        return;

    auto* World = GetWorld();
    if (World == nullptr)
        return;

    AGameModeBase* BaseGameMode = UGameplayStatics::GetGameMode(World);
    if (BaseGameMode)
    {
        if (ALoginGameMode* GameMode = Cast<ALoginGameMode>(BaseGameMode))
        {
            GameMode->HandleSignUpFail();
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
