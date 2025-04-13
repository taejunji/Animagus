// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "../System//MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Character/BaseCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "../AI/MyAIController.h"
#include "../Character/AICharacter.h"
#include "../Character/PlayerCharacter.h"
#include "../PlayerController/Battle_PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../Character/NetworkCharacter.h"
#include "Project_Animagus/UI/MyPlayerHUDWidget.h"
#include "../Skill/SkillsPch.h"

#include "GameFramework/NavMovementComponent.h"
#include "Project_Animagus/Item/BaseItem.h"
#include "Project_Animagus/UI/MyPlayerHUDWidget.h"
#include "Algo/RandomShuffle.h"
#include "Project_Animagus/Item/PowerUpItem.h"
#include "Runtime/Core/Tests/Containers/TestUtils.h"
#include "../Network/ClientPacketHandler.h"


ABattleGameMode::ABattleGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

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

    static ConstructorHelpers::FClassFinder<AAIController> AIController(TEXT("/Game/WorkFolder/AI/AIPlayer/BP_AIController.BP_AIController_C"));
    if (AIController.Succeeded())
    {
        AIControllerClass = AIController.Class;
    }
    static ConstructorHelpers::FClassFinder<APawn> AIPawn(TEXT("/Game/WorkFolder/AI/AIPlayer/BP_AIPlayer.BP_AIPlayer_C"));
    if (AIPawn.Succeeded()) 
    {
        AIPlayerClass = AIPawn.Class; 
    }

    static ConstructorHelpers::FClassFinder<APowerUpItem> Powerupitem(TEXT("/Game/WorkFolder/Bluprints/Item/MyPowerUpItem"));
    if (Powerupitem.Succeeded())
    {
        PowerUpBpclass = Powerupitem.Class;    
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
        //PossessIndex = MyGameInstance->GetMyPlayerIndex();

        elasped_time = 0.0f; 
        GetWorld()->GetTimerManager().ClearTimer(battle_timer_handle); // 타이머가 중지됨 

        //SpawnPlayers(); 

        // 5초 후에 플레이어 입력 활성화
        //FTimerHandle GameStartTimerHandle; 
        //GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this, &ABattleGameMode::ActivateInput, 6.0f, false); 

        // 1초마다 경과시간 호출 함수 타이머 설정
        // GetWorld()->GetTimerManager().SetTimer(battle_timer_handle, this, &ABattleGameMode::PrintElapsedtime, 1.0f, true); 

        // TDOO: 서버에 배틀모드 입장 알림

        CurrentCountdownTime = start_time;
        CurrentRoundTime = 0.0f;

        // 1초마다 CountdownTimerUpdate() 호출
        //GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ABattleGameMode::CountdownTimerUpdate, 1.0f, true);
    }
    
    InitializeArea1SpawnPoints();
    SpawnItemsInArea1();
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
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (PC)
    {
        APawn* AutoPawn = PC->GetPawn();
        if (AutoPawn)
        {
            UE_LOG(LogTemp, Log, TEXT("BattleGameMode: 자동 생성된 Pawn %s 제거함."), *AutoPawn->GetName());
            AutoPawn->Destroy();
        }
    }
    
    // 플레이어 캐릭터들을 SpawnLocations 배열에 따라 스폰함
    SpawnedPlayers.Empty();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: World가 null임."));
        return;
    }
    
    //SpawnLocations 배열에 최소 4개가 있어야 함.
    if (SpawnLocations.Num() < 4)
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: SpawnLocations 수가 충분하지 않음."));
        return;
    }
    
    for (int32 i = 0; i < 1; i++)
    {
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocations[i]);
        SpawnTransform.SetLocation(SpawnLocations[i]);
        // 회전값은 SpawnRotations 배열의 값을 사용함 (있으면)
        if (SpawnRotations.IsValidIndex(i))
        {
            SpawnTransform.SetRotation(SpawnRotations[i].Quaternion());
        }
        else
        {
            SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
        }

        ABaseCharacter* NewCharacter = World->SpawnActor<ABaseCharacter>(DefaultPawnClass, SpawnTransform);
        if (NewCharacter)
        {
            PlayerCharacter = Cast<APlayerCharacter>(NewCharacter);
            UE_LOG(LogTemp, Log, TEXT("BattleGameMode: 플레이어 %d 스폰됨, 위치: %s"), i, *SpawnLocations[i].ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: 플레이어 %d 스폰 실패"), i);
        }

        PC = UGameplayStatics::GetPlayerController(World, 0);
        if (PC)
        {
            PC->Possess(PlayerCharacter);
            PC->DisableInput(PC); // 입력 비활성화

            //if (UCharacterMovementComponent* MovementComp = SpawnedPlayers[PossessIndex]->GetCharacterMovement())
            //{
            //    MovementComp->SetMovementMode(EMovementMode::MOVE_None);   // 공중에서 멈춰서 5초 
            //    MovementComp->SetMovementMode(EMovementMode::MOVE_Falling);// 시작하자마자 낙하하고 5초 
            //}

            UE_LOG(LogTemp, Log, TEXT("BattleGameMode: PlayerController가 인덱스 %d의 캐릭터를 소유함."), PossessIndex);
        }

        // 5초 후에 플레이어 입력 활성화
        FTimerHandle GameStartTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this, &ABattleGameMode::ActivateInput, 6.0f, false);

        // 1초마다 CountdownTimerUpdate() 호출
        GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ABattleGameMode::CountdownTimerUpdate, 1.0f, true);

    }

    // "0"번 플레이어가 아닌 경우 AI 생성하지 않고 나가기
    //if (PossessIndex != 0) return;

    if (AmIHost == false) return;

    uint16 AIId = 101;
    // ** AI를 추가할 경우 -> 0번 플레이어만 만들 것임 ** AI 플레이어 수 설정
    for (int32 i = 1; i < 4; ++i)
    {
        // AI 플레이어 생성 (임의의 `ABaseCharacter`로 가정)
        // FVector AI_SpawnLocation = spawn_transform[i].GetLocation();
        //FRotator AI_SpawnRotation = spawn_transform[i].Rotator();

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocations[i]);
        SpawnTransform.SetRotation(FQuat(SpawnRotations[i]));
        
        // AI 캐릭터 스폰
        AAICharacter* AIChar = GetWorld()->SpawnActor<AAICharacter>(AIPlayerClass,SpawnTransform);
        if (!AIChar) continue;

        auto Movement = AIChar->GetCharacterMovement();
        Movement->bUseControllerDesiredRotation = true;
        Movement->bOrientRotationToMovement = false; 
        //Movement->bUseAccelerationForPaths = false; // MoveTo가 목적지 가까워져도 감속 없이 직선 고속 이동

        // AI 컨트롤러 생성 및 연결
        AMyAIController* AICtrl = GetWorld()->SpawnActor<AMyAIController>(AIControllerClass, SpawnTransform);
        if (AICtrl)
        {
            AICtrl->Possess(AIChar);
            AICtrl->SetControlRotation(FRotator(SpawnTransform.GetRotation()));
            AICtrl->SetIgnoreMoveInput(true);
            AICtrl->SetIgnoreLookInput(true);
            AICtrl->SetSkillCoolTime();
        }

        //SpawnedPlayers.Add(AIChar);
        SpawnedPlayers.Add(static_cast<int32>(AIId++), AIChar);

        Protocol::CS_AI_ENTER_PKT AIPkt;





        SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(AIPkt);
        Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);


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
    if (SpawnedPlayers.Contains(static_cast<int32>(p_id)) == true) return;

    Protocol::PlayerType type = pkt.p_type;

    // 플레이어 스폰 데이터
    FVector SpawnLocation(pkt.x, pkt.y, pkt.z);
    FRotator SpawnRotation(0.0f, pkt.rotation, 0.0f);
    FTransform SpawnTransform(SpawnRotation, SpawnLocation);

    ANetworkCharacter* NewPlayer = World->SpawnActor<ANetworkCharacter>(ANetworkCharacter::StaticClass(), SpawnTransform);
    if (NewPlayer)
    {
        // TODO
        NewPlayer->SetPlayerId(p_id);
        NewPlayer->SetPlayerType(type);

        SpawnedPlayers.Add(static_cast<int32>(p_id), NewPlayer);

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
    ABattle_PlayerController* PlayerController = Cast<ABattle_PlayerController>(PlayerCharacter->GetController()); // 첫 번째 플레이어 컨트롤러 가져오기
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
    for (auto& Item : SpawnedPlayers)
    {
        ABaseCharacter* Players = Item.Value;
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

void ABattleGameMode::SpawnSkill(Protocol::CS_USING_SKILL_PKT& pkt)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnPlayer: World is null"));
        return;
    }

    if (SpawnedPlayers.Contains(static_cast<int32>(pkt.player_id)) == false) return;
    //if (static_cast<int32>(pkt.player_id) == PlayerId) return;    // 자신이 쏜 스킬은 스폰X

    UBaseSkill* Skill = nullptr;

    switch (pkt.s_type)
    {
    case Protocol::SkillType::NONE:
            UE_LOG(LogTemp, Error, TEXT("SpawnSkill: Skill type is NONE"));
            return;
    case Protocol::SkillType::FIREBALL:
            Skill = NewObject<UFireball>(this, UFireball::StaticClass());
            Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
            break;
    case Protocol::SkillType::SHIELD:
            Skill = NewObject<UShieldSkill>(this, UShieldSkill::StaticClass());
            break;
    case Protocol::SkillType::BOUNCE:
            Skill = NewObject<UBounce>(this, UBounce::StaticClass());
            Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
            break;
    case Protocol::SkillType::MAGICMISSILE:
            Skill = NewObject<UMagicMissile>(this, UMagicMissile::StaticClass());
            Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
            break;
    case Protocol::SkillType::SMOKE:
            Skill = NewObject<USmokeSkill>(this, USmokeSkill::StaticClass());
            Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
            break;
    case Protocol::SkillType::RADIAL:
            Skill = NewObject<URadialSkill>(this, URadialSkill::StaticClass());
            Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
            break;
    case Protocol::SkillType::CHANGE:
            Skill = NewObject<UChangeSkill>(this, UChangeSkill::StaticClass());
            Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
            break;
    case Protocol::SkillType::STUN:
            Skill = NewObject<UStun>(this, UStun::StaticClass());
            Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
            break;
    default:
            UE_LOG(LogTemp, Error, TEXT("SpawnSkill: Unknown skill type"));
            return;
    }

    if (Skill && SpawnedPlayers.Contains(static_cast<int32>(pkt.player_id))) {
        Skill->Owner = SpawnedPlayers[static_cast<int32>(pkt.player_id)];
        Skill->ActiveSkill();   // TODO: pkt.pitch, yaw, roll로 회전값 설정 따로 해줘야 함
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

void ABattleGameMode::SetPlayerIndex(uint16 playerIndex)
{
    //PossessIndex = static_cast<int32>(playerIndex);

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        if (APlayerCharacter* MyPlayer = Cast<APlayerCharacter>(PC->GetPawn()))
        {
            MyPlayer->SetPlayerId(playerIndex);
        }
    }
}

void ABattleGameMode::CountdownTimerUpdate()
{

    // DisplayTime이 0보다 큰 경우에만 HUD에 업데이트 (즉, 1초 이상일 때)
    float DisplayTime = FMath::CeilToFloat(CurrentCountdownTime);
    if (DisplayTime > 0)
    {
        // 자신의 HUD도 업데이트
        ABattle_PlayerController* PC = Cast<ABattle_PlayerController>(PlayerCharacter->GetController());
        if (PC && PC->PlayerHUD)
        {
            PC->PlayerHUD->UpdateCountdown(DisplayTime);
        }

        UE_LOG(LogTemp, Log, TEXT("Countdown: %.0f"), DisplayTime);
    }
    else
    {
        ABattle_PlayerController* PC = Cast<ABattle_PlayerController>(PlayerCharacter->GetController());
        if (PC && PC->PlayerHUD)
        {
            PC->PlayerHUD->UpdateCountdown(0.0f);
        }

        GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
        // 라운드 진행 타이머 시작
        GetWorld()->GetTimerManager().SetTimer(RoundTimerHandle, this, &ABattleGameMode::RoundTimerUpdate, 1.0f, true);
        return;
    }

    // 1초 경과 후 CountdownTime 감소
    CurrentCountdownTime -= 1.0f;
}

void ABattleGameMode::RoundTimerUpdate()
{
    CurrentRoundTime += 1.0f;
    //for (auto& Item : SpawnedPlayers)
    //{
    //    ABaseCharacter* Player = Item.Value;
    //    if (Player && Player->GetController())
    //    {
    //        ABattle_PlayerController* PC = Cast<ABattle_PlayerController>(Player->GetController());
    //        if (PC && PC->PlayerHUD)
    //        {
    //            PC->PlayerHUD->UpdateRoundTime(CurrentRoundTime);
    //        }
    //    }
    //}

    if (PlayerCharacter != nullptr)
    {
        ABattle_PlayerController* PC = Cast<ABattle_PlayerController>(PlayerCharacter->GetController());
        if (PC && PC->PlayerHUD)
        {
            PC->PlayerHUD->UpdateRoundTime(CurrentRoundTime);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Round Time: %.0f"), CurrentRoundTime);
}

void ABattleGameMode::SpawnItemsInArea1()
{
    // 예시로 영역1에 10개의 아이템 스폰
    const int32 NumItemsToSpawn = 30;
    if (Area1SpawnPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: No spawn points available."));
        return;
    }

    // 만약 NumItemsToSpawn이 전체 스폰 포인트보다 많으면, 경고 로그 출력
    if (NumItemsToSpawn > Area1SpawnPoints.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: Not enough spawn points available. Reducing item count."));
    }

    // SpawnedItems 배열 초기화
    SpawnedItems.Empty();

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: World is null."));
        return;
    }

    // 지역 spawn 포인트 배열 복사 및 셔플
    TArray<FVector> LocalSpawnPoints = Area1SpawnPoints;
    
    Test::Shuffle(LocalSpawnPoints);
    
    int32 ItemsToSpawn = FMath::Min(NumItemsToSpawn, LocalSpawnPoints.Num());

    for (int32 i = 0; i < ItemsToSpawn; i++)
    {
        FVector SpawnLocation = LocalSpawnPoints[i];
        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        // 필요에 따라 SpawnParams.Owner 또는 Instigator 설정

        ABaseItem* NewItem = World->SpawnActor<ABaseItem>(PowerUpBpclass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewItem)
        {
            SpawnedItems.Add(NewItem);
            UE_LOG(LogTemp, Log, TEXT("SpawnItemsInArea1: Spawned item at %s"), *SpawnLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: Failed to spawn item at index %d"), i);
        }
    }
}

void ABattleGameMode::InitializeArea1SpawnPoints()
{
    // 기존 좌표 배열 초기화
    Area1SpawnPoints.Empty();

    // 예시로 몇 개의 하드코딩 좌표를 추가 (실제 프로젝트에서는 150개 정도의 좌표를 입력)
    Area1SpawnPoints.Add(FVector(-12400.f, -600.f, 66.f));
    Area1SpawnPoints.Add(FVector(-13000.f, 500.f, 66.f));
    Area1SpawnPoints.Add(FVector(-11000.f, -1500.f, 66.f));
    Area1SpawnPoints.Add(FVector(-11500.f, 800.f, 66.f));
    Area1SpawnPoints.Add(FVector(-12000.f, -1000.f, 66.f));

    Area1SpawnPoints.Add(FVector(12400.f, -600.f, 66.f));
    Area1SpawnPoints.Add(FVector(13000.f, 500.f, 66.f));
    Area1SpawnPoints.Add(FVector(11000.f, -1500.f, 66.f));
    Area1SpawnPoints.Add(FVector(11500.f, 800.f, 66.f));
    Area1SpawnPoints.Add(FVector(12000.f, -1000.f, 66.f)); 

    Area1SpawnPoints.Add(FVector(12400.f, 600.f, 66.f));
    Area1SpawnPoints.Add(FVector(13000.f, -500.f, 66.f));
    Area1SpawnPoints.Add(FVector(11000.f, 1500.f, 66.f));
    Area1SpawnPoints.Add(FVector(11500.f, -800.f, 66.f));
    Area1SpawnPoints.Add(FVector(12000.f, 1000.f, 66.f)); 

    Area1SpawnPoints.Add(FVector(-12400.f, 600.f, 66.f));
    Area1SpawnPoints.Add(FVector(-13000.f, -500.f, 66.f));
    Area1SpawnPoints.Add(FVector(-11000.f, 1500.f, 66.f));
    Area1SpawnPoints.Add(FVector(-11500.f, -800.f, 66.f));
    Area1SpawnPoints.Add(FVector(-12000.f, 1000.f, 66.f)); 

    Area1SpawnPoints.Add(FVector(-12325.f, 3641.f, 66.f));
    Area1SpawnPoints.Add(FVector(-10195.f, -6392.f, 66.f));
    Area1SpawnPoints.Add(FVector(-9100.f, 6392.f, 66.f));
    
  
    Area1SpawnPoints.Add(FVector(-8300.f, 8100.f, 66.f));
    Area1SpawnPoints.Add(FVector(-8300.f, 10800.f, 66.f));
    Area1SpawnPoints.Add(FVector(-8300.f, 13500.f, 66.f));

    Area1SpawnPoints.Add(FVector(8300.f, 8100.f, 66.f));
    Area1SpawnPoints.Add(FVector(8300.f, 10800.f, 66.f));
    Area1SpawnPoints.Add(FVector(8300.f, 13500.f, 66.f));

    Area1SpawnPoints.Add(FVector(8300.f, -8100.f, 66.f));
    Area1SpawnPoints.Add(FVector(8300.f, -10800.f, 66.f));
    Area1SpawnPoints.Add(FVector(8300.f, -13500.f, 66.f));

    Area1SpawnPoints.Add(FVector(-8300.f, -8100.f, 66.f));
    Area1SpawnPoints.Add(FVector(-8300.f, -10800.f, 66.f));
    Area1SpawnPoints.Add(FVector(-8300.f, -13500.f, 66.f));
    
    Area1SpawnPoints.Add(FVector(-5400.f, 8100.f, 66.f));
    Area1SpawnPoints.Add(FVector(-5400.f, 10800.f, 66.f));
    Area1SpawnPoints.Add(FVector(-5400.f, 13500.f, 66.f));

    Area1SpawnPoints.Add(FVector(5400.f, 8100.f, 66.f));
    Area1SpawnPoints.Add(FVector(5400.f, 10800.f, 66.f));
    Area1SpawnPoints.Add(FVector(5400.f, 13500.f, 66.f));

    Area1SpawnPoints.Add(FVector(-5400.f, -8100.f, 66.f));
    Area1SpawnPoints.Add(FVector(-5400.f, -10800.f, 66.f));
    Area1SpawnPoints.Add(FVector(-5400.f, -13500.f, 66.f));

    Area1SpawnPoints.Add(FVector(5400.f, -8100.f, 66.f));
    Area1SpawnPoints.Add(FVector(5400.f, -10800.f, 66.f));
    Area1SpawnPoints.Add(FVector(5400.f, -13500.f, 66.f));
    
    Area1SpawnPoints.Add(FVector(1394.f, 11911.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(2044.f, 10873.f, 66.f));
    Area1SpawnPoints.Add(FVector(4469.f, 9526.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(5479.f, 10049.f, 66.f)); 
    
    Area1SpawnPoints.Add(FVector(-1394.f, -11911.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-2044.f, -10873.f, 66.f));
    Area1SpawnPoints.Add(FVector(-4469.f, -9526.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-5479.f, -10049.f, 66.f)); 

    Area1SpawnPoints.Add(FVector(1394.f, -11911.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(2044.f, -10873.f, 66.f));
    Area1SpawnPoints.Add(FVector(4469.f, -9526.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(5479.f, -10049.f, 66.f)); 

    Area1SpawnPoints.Add(FVector(-1394.f, 11911.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-2044.f, 10873.f, 66.f));
    Area1SpawnPoints.Add(FVector(-4469.f, 9526.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-5479.f, 10049.f, 66.f));
    
    Area1SpawnPoints.Add(FVector(-12031.f, 4344.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-10562.f, 6102.f, 66.f));
    Area1SpawnPoints.Add(FVector(-9121.f, 7897.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-9565.f, 4504.f, 66.f));

    Area1SpawnPoints.Add(FVector(12031.f, 4344.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(10562.f, 6102.f, 66.f));
    Area1SpawnPoints.Add(FVector(9121.f, 7897.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(9565.f, 4504.f, 66.f));

    Area1SpawnPoints.Add(FVector(12031.f, -4344.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(10562.f, -6102.f, 66.f));
    Area1SpawnPoints.Add(FVector(9121.f, -7897.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(9565.f, -4504.f, 66.f));

    Area1SpawnPoints.Add(FVector(-12031.f, -4344.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-10562.f, -6102.f, 66.f));
    Area1SpawnPoints.Add(FVector(-9121.f, -7897.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-9565.f, -4504.f, 66.f));

    Area1SpawnPoints.Add(FVector(-7845.f, -115.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-8151.f, -1855.f, 66.f));
    Area1SpawnPoints.Add(FVector(-8806.f, -2018.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-7416.f, -7377.f, 66.f)); 

    Area1SpawnPoints.Add(FVector(7845.f, -115.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(8151.f, -1855.f, 66.f));
    Area1SpawnPoints.Add(FVector(8806.f, -2018.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(7416.f, -7377.f, 66.f)); 

    Area1SpawnPoints.Add(FVector(7845.f, 115.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(8151.f, 1855.f, 66.f));
    Area1SpawnPoints.Add(FVector(8806.f, 2018.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(7416.f, 7377.f, 66.f)); 

    Area1SpawnPoints.Add(FVector(-7845.f, 115.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-8151.f, 1855.f, 66.f));
    Area1SpawnPoints.Add(FVector(-8806.f, 2018.f, 66.f)); 
    Area1SpawnPoints.Add(FVector(-7416.f, 7377.f, 66.f)); 
    
    UE_LOG(LogTemp, Log, TEXT("InitializeRegion1SpawnPoints: Initialized %d spawn points."), Area1SpawnPoints.Num());
}