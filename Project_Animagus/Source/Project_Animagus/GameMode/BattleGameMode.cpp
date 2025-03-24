// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "../System//MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Character/BaseCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "../AI/MyAIController.h"
#include "../Character/AICharacter.h"
#include "../PlayerController/Battle_PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/NetworkCharacter.h"


ABattleGameMode::ABattleGameMode()
{
    PrimaryActorTick.bCanEverTick = false;

    // BluePirnt Class인 BP_Player, BP_PlayerController의 정보를 생성자에서 읽어서 게임모드에 설정한다.
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/WorkFolder/Bluprints/BP_Player.BP_Player_C"));
    if (PlayerPawn.Succeeded())
    {
        DefaultPawnClass = PlayerPawn.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("디폴트 폰 로드 실패")); 

    static ConstructorHelpers::FClassFinder<APlayerController> PController(TEXT("/Game/WorkFolder/Controller/BP_Battle_PlayerController.BP_Battle_PlayerController_C"));
    if (PController.Succeeded())
    {
        PlayerControllerClass = PController.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("플레이어 컨트롤러 로드 실패"));

    static ConstructorHelpers::FClassFinder<AAIController> AIController(TEXT("/Game/WorkFolder/Bluprints/AIPlayer/BP_AIController.BP_AIController_C"));
    if (AIController.Succeeded())
    {
        AIControllerClass = AIController.Class;
    }
    static ConstructorHelpers::FClassFinder<APawn> AIPawn(TEXT("/Game/WorkFolder/Bluprints/AIPlayer/BP_AIPlayer.BP_AIPlayer_C"));
    if (AIPawn.Succeeded()) 
    {
        AIPlayerClass = AIPawn.Class; 
    }

    // 플레이어 ID(0~3)와 스폰 위치를 매핑
    spawn_transform.Add(0, FTransform(FRotator(0, 0, 0), FVector(-13500.0f, 0.0f, 800.f))); // Spawn_0
    spawn_transform.Add(1, FTransform(FRotator(0, 90, 0), FVector(0.0f, -13500.0f, 800.f))); // Spawn_1
    spawn_transform.Add(2, FTransform(FRotator(0, 180, 0), FVector(13500.0f, 0.0f, 800.f))); // Spawn_2
    spawn_transform.Add(3, FTransform(FRotator(0, 270, 0), FVector(0.0f, 13500.0f, 800.f))); // Spawn_3
    
    // SpawnLocations 기본값 설정 (에디터에서 재조정 가능)
    SpawnLocations.Add(FVector(-13500.0f, 0.0f, 800.f));
    SpawnLocations.Add(FVector(0.0f, -13500.0f, 800.f));
    SpawnLocations.Add(FVector(13500.0f, 0.0f, 800.f));
    SpawnLocations.Add(FVector(0.0f, 13500.0f, 800.f));

    SpawnRotations.Add(FRotator(0.f, 0.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 90.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 180.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 270.f, 0.f));
    
    PossessIndex = 0; // 기본적으로 0번 플레이어를 소유하도록 설정

}

void ABattleGameMode::StartPlay()
{
    Super::StartPlay();

    InitBattleMode();
}

void ABattleGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
 
    Cast<UMyGameInstance>(GWorld->GetGameInstance())->HandleRecvPackets();
}

void ABattleGameMode::InitBattleMode()
{
    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));  
    if (MyGameInstance)
    {
        elasped_time = 0.0f; 
        GetWorld()->GetTimerManager().ClearTimer(battle_timer_handle); // 타이머가 중지됨 

        SpawnPlayers(); 

        // 5초 후에 플레이어 입력 활성화
        FTimerHandle GameStartTimerHandle; 
        GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this, &ABattleGameMode::ActivateInput, start_time, false); 

        // 1초마다 경과시간 호출 함수 타이머 설정
        // GetWorld()->GetTimerManager().SetTimer(battle_timer_handle, this, &ABattleGameMode::PrintElapsedtime, 1.0f, true); 

        // 서버에 배틀모드 입장 알림

    }
}

void ABattleGameMode::SpawnPlayers()
{
    // 먼저 자동으로 생성된 Pawn이 있다면 제거함
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: World가 null임."));
        return;
    }
    

    // APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    // if (PC)
    // {
    //     APawn* AutoPawn = PC->GetPawn();
    //     if (AutoPawn)
    //     {
    //         UE_LOG(LogTemp, Log, TEXT("BattleGameMode: 자동 생성된 Pawn %s 제거함."), *AutoPawn->GetName());
    //         AutoPawn->Destroy();
    //     }
    // }
    //
    // // 플레이어 캐릭터들을 SpawnLocations 배열에 따라 스폰함
    // SpawnedPlayers.Empty();
    // if (!World)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: World가 null임."));
    //     return;
    // }
    //
    // // SpawnLocations 배열에 최소 4개가 있어야 함.
    // if (SpawnLocations.Num() < 4)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: SpawnLocations 수가 충분하지 않음."));
    //     return;
    // }
    //
    // for (int32 i = 0; i < 1; i++)
    // {
    //     FTransform SpawnTransform;
    //     SpawnTransform.SetLocation(SpawnLocations[i]);
    //     SpawnTransform.SetLocation(SpawnLocations[i]);
    //     // 회전값은 SpawnRotations 배열의 값을 사용함 (있으면)
    //     if (SpawnRotations.IsValidIndex(i))
    //     {
    //         SpawnTransform.SetRotation(SpawnRotations[i].Quaternion());
    //     }
    //     else
    //     {
    //         SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
    //     }
    //     ABaseCharacter* NewCharacter = World->SpawnActor<ABaseCharacter>(DefaultPawnClass, SpawnTransform);
    //     if (NewCharacter)
    //     {
    //         SpawnedPlayers.Add(NewCharacter);
    //         UE_LOG(LogTemp, Log, TEXT("BattleGameMode: 플레이어 %d 스폰됨, 위치: %s"), i, *SpawnLocations[i].ToString());
    //     }
    //     else
    //     {
    //         UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: 플레이어 %d 스폰 실패"), i);
    //     }
    // }
    //
    // // PossessIndex 안전 검사 후, 해당 인덱스의 캐릭터를 소유하도록 함
    // if (SpawnedPlayers.IsValidIndex(PossessIndex))
    // {
    //     PC = UGameplayStatics::GetPlayerController(World, 0);
    //     if (PC)
    //     {
    //         PC->Possess(SpawnedPlayers[PossessIndex]);
    //         PC->DisableInput(PC); // 입력 비활성화
    //
    //         //if (UCharacterMovementComponent* MovementComp = SpawnedPlayers[PossessIndex]->GetCharacterMovement())
    //         //{
    //         //    MovementComp->SetMovementMode(EMovementMode::MOVE_None);   // 공중에서 멈춰서 5초 
    //         //    MovementComp->SetMovementMode(EMovementMode::MOVE_Falling);// 시작하자마자 낙하하고 5초 
    //         //}
    //
    //         UE_LOG(LogTemp, Log, TEXT("BattleGameMode: PlayerController가 인덱스 %d의 캐릭터를 소유함."), PossessIndex);
    //     }
    // }
    // else
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: PossessIndex %d가 유효하지 않음."), PossessIndex);
    // }


    // "0"번 플레이어가 아닌 경우 AI 생성하지 않고 나가기
    if (PossessIndex != 0) return;

    // ** AI를 추가할 경우 -> 0번 플레이어만 만들 것임 ** AI 플레이어 수 설정
    for (int32 i = 1; i < 4; ++i)
    {
        // AI 플레이어 생성 (임의의 `ABaseCharacter`로 가정)
        // FVector AI_SpawnLocation = spawn_transform[i].GetLocation();
        //FRotator AI_SpawnRotation = spawn_transform[i].Rotator();

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocations[i]);
        SpawnTransform.SetLocation(SpawnLocations[i]);
        
        // AI 캐릭터 스폰
        AAICharacter* AIChar = GetWorld()->SpawnActor<AAICharacter>(AIPlayerClass,SpawnTransform);
        if (!AIChar) continue;

        // AI 컨트롤러 생성 및 연결
        AMyAIController* AICtrl = GetWorld()->SpawnActor<AMyAIController>(AIControllerClass, SpawnTransform);
        if (AICtrl)
        {
            AICtrl->Possess(AIChar);
            AICtrl->SetControlRotation(FRotator(SpawnTransform.GetRotation()));
            AICtrl->SetIgnoreMoveInput(true);
            AICtrl->SetIgnoreLookInput(true);
        }

        SpawnedPlayers.Add(AIChar);
    }
}

void ABattleGameMode::SpawnPlayer(Protocol::SC_SPAWN_PKT& pkt)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnPlayer: World is null"));
        return;
    }

    // TODO : 플레이어 ID 중복체크
    uint16 p_id = pkt.player_id;
    Protocol::PlayerType type = pkt.p_type;

    // 플레이어 스폰 데이터
    FVector SpawnLocation(pkt.x, pkt.y, pkt.z);
    FRotator SpawnRotation(0.0f, pkt.rotation, 0.0f);
    FTransform SpawnTransform(SpawnRotation, SpawnLocation);

    ANetworkCharacter* NewPlayer = World->SpawnActor<ANetworkCharacter>(ABaseCharacter::StaticClass(), SpawnTransform);
    if (NewPlayer)
    {
        // TODO
        NewPlayer->SetPlayerID(p_id);
        NewPlayer->SetPlayerType(type);

        SpawnedPlayers.Add(NewPlayer);

        UE_LOG(LogTemp, Log, TEXT("SpawnPlayer: Spawned player %d at (%f, %f, %f)"),
            pkt.player_id, SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnPlayer: Failed to spawn player actor"));
    }
}

void ABattleGameMode::ActivateInput()
{
    ABattle_PlayerController* PlayerController = Cast<ABattle_PlayerController>(SpawnedPlayers[PossessIndex]->GetController()); // 첫 번째 플레이어 컨트롤러 가져오기
    if (PlayerController)
    {
        PlayerController->EnableInput(PlayerController);

        UCharacterMovementComponent* CharacterMovement = Cast<UCharacterMovementComponent>(PlayerController->GetPawn()->GetMovementComponent());
        if (CharacterMovement)
        {
            CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking); // 다시 걷기 모드로 전환
        }
    }

    // "0"번 플레이어가 아닌 경우 AI 생성하지 않고 나가기
    if (PossessIndex != 0) return;

    // **AI들의 Behavior Tree 실행**
    for (ABaseCharacter* Players : SpawnedPlayers)
    {
        if (AAICharacter* AICastedChar = Cast<AAICharacter>(Players))
        {
            AMyAIController* AICtrl = Cast<AMyAIController>(AICastedChar->GetController());
            if (AICtrl)
            {
                AICtrl->StartBehaviorTree();
                AICtrl->SetIgnoreMoveInput(false);  // AI 입력 활성화
                AICtrl->SetIgnoreLookInput(false);  // AI 회전 활성화
            }

            //if (UCharacterMovementComponent* MovementComp = AIChar->GetCharacterMovement())
            //{
            //    MovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
            //}
        }
    }
}

void ABattleGameMode::PrintElapsedtime()
{
    // 경과 시간을 출력
    elasped_time += 1.0f; 
     
    FString DebugMessage = FString::Printf(TEXT("라운드 경과 시간: %f초"), elasped_time);
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, DebugMessage); 

    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MyGameInstance)
    {
        // 5초마다 라운드 증가 [ 그냥 테스트 코드 ]
        if ((int)elasped_time % 5 == 0)
        {
            MyGameInstance->round_count++;
        }
        MyGameInstance->PrintGameInstanceData(); 
    }
}
