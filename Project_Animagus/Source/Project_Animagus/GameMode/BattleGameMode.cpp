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
#include "Algo/RandomShuffle.h"
#include "Project_Animagus/Actor/ItemBox/Item_Box_Base.h"
#include "Project_Animagus/Actor/ItemBox/Item_Box_High.h"
#include "Project_Animagus/Item/PowerUpItem.h"
#include "Runtime/Core/Tests/Containers/TestUtils.h"
#include "../Network/ClientPacketHandler.h"
#include "../Actor/Zones/AttractionZone.h"


ABattleGameMode::ABattleGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    // BluePirnt Class인 BP_Player, BP_PlayerController의 정보를 생성자에서 읽어서 게임모드에 설정한다.
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/WorkFolder/Bluprints/BP_Player.BP_Player_C"));
    if (PlayerPawn.Succeeded())
    {
        DefaultPawnClass = PlayerPawn.Class;
        UE_LOG(LogTemp, Warning, TEXT("폰 로드 성공"));
    }
    else UE_LOG(LogTemp, Warning, TEXT("디폴트 폰 로드 실패"));

    static ConstructorHelpers::FClassFinder<APawn> NetPlayerPawn(TEXT("/Game/WorkFolder/Bluprints/MyNetworkCharacter"));
    if (NetPlayerPawn.Succeeded())
    {
        NetPawnClass = NetPlayerPawn.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("디폴트 폰 로드 실패"));


    static ConstructorHelpers::FClassFinder<APlayerController> PController(TEXT("/Game/WorkFolder/Controller/BP_Battle_PlayerController.BP_Battle_PlayerController_C"));
    if (PController.Succeeded())
    {
        PlayerControllerClass = PController.Class;
        UE_LOG(LogTemp, Warning, TEXT("플레이어 컨트롤러 로드 성공"));
    }
    else UE_LOG(LogTemp, Warning, TEXT("플레이어 컨트롤러 로드 실패"));
    
    UE_LOG(LogTemp, Warning, TEXT("AI 컨트롤러 로드 전"));
    
    // static ConstructorHelpers::FClassFinder<AAIController> AIController(TEXT("/Game/WorkFolder/AI/AIPlayer/BP_AIController.BP_AIController_C"));
    // if (AIController.Succeeded())
    // {
    //     AIControllerClass = AIController.Class;
    //     UE_LOG(LogTemp, Warning, TEXT("AI 컨트롤러 로드 성공"));
    // }
    // else UE_LOG(LogTemp, Warning, TEXT("AI 컨트롤러 로드 실패")); 
    //
    UE_LOG(LogTemp, Warning, TEXT("AI 컨트롤러 로드 후"));
    
    // static ConstructorHelpers::FClassFinder<APawn> AIPawn(TEXT("/Game/WorkFolder/AI/AIPlayer/BP_AIPlayer.BP_AIPlayer_C"));
    // if (AIPawn.Succeeded()) 
    // {
    //     AIPlayerClass = AIPawn.Class;
    //     UE_LOG(LogTemp, Warning, TEXT("AI 폰 로드 성공"));
    // }
    // else UE_LOG(LogTemp, Warning, TEXT("AI 폰 로드 실패")); 

    UE_LOG(LogTemp, Warning, TEXT("AI 폰 로드 후/파워 아이템 로드 전"));
    
    static ConstructorHelpers::FClassFinder<APowerUpItem> Powerupitem(TEXT("/Game/WorkFolder/Bluprints/Item/MyPowerUpItem"));
    if (Powerupitem.Succeeded())
    {
        PowerUpBpclass = Powerupitem.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("파워업 아이템 로드 실패"));

    UE_LOG(LogTemp, Warning, TEXT("파워 아이템 로드 후 / 아이템 박스 로드 전"));
    
    static ConstructorHelpers::FClassFinder<AItem_Box_Base> ItemboxBp(TEXT("/Game/WorkFolder/Bluprints/Actor/MyItem_Box_Base"));
    if (ItemboxBp.Succeeded())
    {
        ItemBoxBpclass = ItemboxBp.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("아이템박스_기본 로드 실패"));

    static ConstructorHelpers::FClassFinder<AAttractionZone> AttractionZoneBp(TEXT("/Game/WorkFolder/Bluprints/Spiders/BP_AttractionZone"));
    if (AttractionZoneBp.Succeeded())
    {
        AttractionBpclass = AttractionZoneBp.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("거미존 로드 실패"));

    static ConstructorHelpers::FClassFinder<AItem_Box_High> ItemboxplusBp(TEXT("/Game/WorkFolder/Bluprints/Actor/MyItem_Box_High"));
    if (ItemboxplusBp.Succeeded())
    {
        ItemBoxHighBpclass = ItemboxplusBp.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("아이템박스_노랑 로드 실패"));

    UE_LOG(LogTemp, Warning, TEXT("아이템 박스 로드 후"));
    
    // 플레이어 ID(0~3)와 스폰 위치를 매핑
    spawn_transform.Add(0, FTransform(FRotator(0, 0, 0), FVector(-13500.0f, 0.0f, 800.f))); // Spawn_0
    spawn_transform.Add(1, FTransform(FRotator(0, 90, 0), FVector(0.0f, -13500.0f, 800.f))); // Spawn_1
    spawn_transform.Add(2, FTransform(FRotator(0, 180, 0), FVector(13500.0f, 0.0f, 800.f))); // Spawn_2
    spawn_transform.Add(3, FTransform(FRotator(0, 270, 0), FVector(0.0f, 13500.0f, 800.f))); // Spawn_3
    
    // SpawnLocations 기본값 설정 (에디터에서 재조정 가능)
    SpawnLocations.Add(FVector(-12850.0f, -2000.0f, 800.f));
    SpawnLocations.Add(FVector(2000.0f, -12850.0f, 800.f));
    SpawnLocations.Add(FVector(12850.0f, 2000.0f, 800.f));
    SpawnLocations.Add(FVector(2000.0f, 12850.0f, 800.f));

    SpawnLocations.Add(FVector(-12850.0f, 2000.0f, 800.f));
    SpawnLocations.Add(FVector(-2000.0f, -12850.0f, 800.f));
    SpawnLocations.Add(FVector(12850.0f, -2000.0f, 800.f));
    SpawnLocations.Add(FVector(-2000.0f, 12850.0f, 800.f));

    SpawnRotations.Add(FRotator(0.f, 0.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 90.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 180.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 270.f, 0.f));

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
    
    if (BackgroundMusic)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), BackgroundMusic);
    }
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
    
    IndexingSpawnedPlayers.Empty();
    SpawnedPlayers.Empty();

    AreaSpawnPoints.Empty();
    SpawnedItems.Empty();

    InitializeArea1SpawnPoints();
    InitializeArea2SpawnPoints();
    InitializeArea3SpawnPoints();

    //SpawnItemsInArea1();
    //SpawnItemsInArea3();
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

    //SpawnLocations 배열에 최소 8개가 있어야 함.
    if (SpawnLocations.Num() < 8)
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: SpawnLocations 수가 충분하지 않음."));
        return;
    }

    {
        UE_LOG(LogTemp, Warning, TEXT("PossessIndex: %d"), PossessIndex);

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocations[PossessIndex]);
        // 회전값은 SpawnRotations 배열의 값을 사용함 (있으면)
        if (SpawnRotations.IsValidIndex(PossessIndex))
        {
            SpawnTransform.SetRotation(SpawnRotations[PossessIndex].Quaternion());
        }
        else
        {
            SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
        }

        ABaseCharacter* NewCharacter = World->SpawnActor<ABaseCharacter>(DefaultPawnClass, SpawnTransform);
        if (NewCharacter)
        {
            PlayerCharacter = Cast<APlayerCharacter>(NewCharacter);
            UE_LOG(LogTemp, Log, TEXT("BattleGameMode: 플레이어 %d 스폰됨, 위치: %s"), PossessIndex, *SpawnLocations[PossessIndex].ToString());

            IndexingSpawnedPlayers.Add(PlayerCharacter);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: 플레이어 %d 스폰 실패"), PossessIndex);
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

        FTransform AttractionSpawnTransform;
        AttractionSpawnTransform.SetLocation(FVector(0.f, 0.f, 236.f));
        AAttractionZone* AttractionZone = World->SpawnActor<AAttractionZone>(AttractionBpclass, AttractionSpawnTransform);
        AttractionZone->OwnerCharacter = PlayerCharacter;
    }

    // "0"번 플레이어가 아닌 경우 AI 생성하지 않고 나가기
    //if (PossessIndex != 0) return;

    if (AmIHost == false) return;

    uint16 AIId = 101;
    // ** AI를 추가할 경우 -> 0번 플레이어만 만들 것임 ** AI 플레이어 수 설정
    for (int32 i = 1; i < 2; ++i)
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

        AIChar->bUseControllerRotationYaw = false;

        auto Movement = AIChar->GetCharacterMovement(); 
        Movement->bOrientRotationToMovement = true; 
        Movement->bUseControllerDesiredRotation = false; 
        //Movement->bUseAccelerationForPaths = false; // MoveTo가 목적지 가까워져도 감속 없이 직선 고속 이동
        
        AIChar->SetPlayerId(AIId);


        //SpawnedPlayers.Add(AIChar);
        SpawnedPlayers.Add(static_cast<int32>(AIId), AIChar);

        IndexingSpawnedPlayers.Add(AIChar);

        FTransform AttractionSpawnTransform;
        AttractionSpawnTransform.SetLocation(FVector(0.f, 0.f, 236.f));
        AAttractionZone* AttractionZone = World->SpawnActor<AAttractionZone>(AttractionBpclass, AttractionSpawnTransform);
        AttractionZone->OwnerCharacter = AIChar;

        Protocol::CS_AI_ENTER_PKT AIPkt;

        AIPkt.player_id = Cast<UMyGameInstance>(GWorld->GetGameInstance())->MyPlayerId;
        AIPkt.ai_id = AIId;
        AIPkt.p_type = AIChar->GetPlayerType();
        AIPkt.x = SpawnLocations[i].X;
        AIPkt.y = SpawnLocations[i].Y;
        AIPkt.z = SpawnLocations[i].Z;
        AIPkt.rotation = SpawnRotations[i].Yaw;

        SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(AIPkt);
        Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);

        AIId++;
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

    if (p_id >= 100)
    {
        UE_LOG(LogTemp, Warning, TEXT("AI Player Spawn: %d"), p_id);
    }

    Protocol::PlayerType type = pkt.p_type;

    // 플레이어 스폰 데이터
    FVector SpawnLocation(pkt.x, pkt.y, pkt.z);
    FRotator SpawnRotation(0.0f, pkt.rotation, 0.0f);
    FTransform SpawnTransform(SpawnRotation, SpawnLocation);


    ANetworkCharacter* NewPlayer = World->SpawnActor<ANetworkCharacter>(NetPawnClass, SpawnTransform);
    if (NewPlayer)
    {
        // TODO
        NewPlayer->SetPlayerId(p_id);
        NewPlayer->SetPlayerType(type);

        SpawnedPlayers.Add(static_cast<int32>(p_id), NewPlayer);

        IndexingSpawnedPlayers.Add(NewPlayer);

        FTransform AttractionSpawnTransform;
        AttractionSpawnTransform.SetLocation(FVector(0.f, 0.f, 236.f));
        AAttractionZone* AttractionZone = World->SpawnActor<AAttractionZone>(AttractionBpclass, AttractionSpawnTransform);
        AttractionZone->OwnerCharacter = NewPlayer;

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
    if (AmIHost == false) return;

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

    if (StartSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), StartSound);
    }
}

void ABattleGameMode::MoveOtherPlayer(Protocol::CS_MOVE_PKT& pkt)
{
    const uint16 playerId = pkt.player_info.player_id;
    if (SpawnedPlayers.Find(playerId) == nullptr)
        return;

    //if (playerId == PossessIndex)
    //    return;

    Protocol::PlayerInfo Info = pkt.player_info;
    FVector Location(Info.x, Info.y, Info.z);
    FRotator Rotation(0, Info.rotation, 0);
    Protocol::PlayerState State = Info.player_state;

    ANetworkCharacter* Player = Cast<ANetworkCharacter>(SpawnedPlayers[playerId]);

    Player->SetActorLocation(Location);
    Player->SetActorRotation(Rotation);
    Player->GetCharacterMovement()->Velocity = FVector(0, Info.speed_2d, Info.speed_z);

    if (Player->GetPlayerId() < 100) {
        //UE_LOG(LogTemp, Warning, TEXT("OtherAccel: %f - %d"), Player->GetCharacterMovement()->GetCurrentAcceleration().Size2D(), Player->GetPlayerId());
    }

    if (State == Protocol::PlayerState::MOVE_STATE_JUMP)
        Player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
    else if (State == Protocol::PlayerState::MOVE_STATE_RUN)
        Player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    else if (State == Protocol::PlayerState::MOVE_STATE_IDLE)
        Player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

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
    ABaseCharacter* Player = SpawnedPlayers[static_cast<int32>(pkt.player_id)];
    //if (static_cast<int32>(pkt.player_id) == PlayerId) return;    // 자신이 쏜 스킬은 스폰X
    if (Player == nullptr) return;

    UBaseSkill* Skill = nullptr;

    switch (pkt.s_type)
    {
    case Protocol::SkillType::NONE:
        UE_LOG(LogTemp, Error, TEXT("SpawnSkill: Skill type is NONE"));
        return;
    case Protocol::SkillType::FIREBALL:
        Skill = NewObject<UFireball>(this, UFireball::StaticClass());
        Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    case Protocol::SkillType::SHIELD:
        Skill = NewObject<UShieldSkill>(this, UShieldSkill::StaticClass());
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    case Protocol::SkillType::BOUNCE:
        Skill = NewObject<UBounce>(this, UBounce::StaticClass());
        Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    case Protocol::SkillType::MAGICMISSILE:
        Skill = NewObject<UMagicMissile>(this, UMagicMissile::StaticClass());
        Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    case Protocol::SkillType::SMOKE:
        Skill = NewObject<USmokeSkill>(this, USmokeSkill::StaticClass());
        Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    case Protocol::SkillType::RADIAL:
        Skill = NewObject<URadialSkill>(this, URadialSkill::StaticClass());
        Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    case Protocol::SkillType::CHANGE:
        Skill = NewObject<UChangeSkill>(this, UChangeSkill::StaticClass());
        Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    case Protocol::SkillType::STUN:
        Skill = NewObject<UStun>(this, UStun::StaticClass());
        Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    case Protocol::SkillType::HASTE:
        Skill = NewObject<UHasteSkill>(this, UHasteSkill::StaticClass());
        Skill->SetSkillRotation(pkt.pitch, pkt.yaw, pkt.roll);
        Skill->UpgradeSkill(Player->PowerUpLevel);
        break;
    default:
        UE_LOG(LogTemp, Error, TEXT("SpawnSkill: Unknown skill type"));
        return;
    }

    if (Skill) {
        Skill->Owner = Player;
        Skill->ActiveSkill();   // TODO: pkt.pitch, yaw, roll로 회전값 설정 따로 해줘야 함
    }
}

void ABattleGameMode::SpawnItem(Protocol::SC_SPAWN_ITEM_PKT& pkt)
{
    uint16 ZoneIndex = pkt.zone_index;
    if (ZoneIndex >= 2)
    {
        SpawnItemsInArea3(pkt);
        return;
    }

    int32 SpawnIndex[20];
    int32 ItemLevel[20];

    const uint16 Count = pkt.item_count;
    for (int32 i = 0; i < Count; ++i)
    {
        SpawnIndex[i] = static_cast<int32>(pkt.spawn_index[i]);
        ItemLevel[i] = static_cast<int32>(pkt.item_level[i]);
    }

    //UE_LOG(LogTemp, Warning, TEXT("%d - %d"), SpawnIndex[19], ItemLevel[19]);

    if (AreaSpawnPoints[ZoneIndex].Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: No spawn points available."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: World is null."));
        return;
    }

    // 지역 spawn 포인트 배열 복사 및 셔플

    //int32 ItemsToSpawn = FMath::Min(NumItemsToSpawn, Area1SpawnPoints.Num());

    for (int32 i = 0; i < Count; ++i)
    {
        int32 index = SpawnIndex[i];
        FVector SpawnLocation = AreaSpawnPoints[ZoneIndex][index];
        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        // 필요에 따라 SpawnParams.Owner 또는 Instigator 설정

        AItem_Box_Base* NewItem = World->SpawnActor<AItem_Box_Base>(ItemBoxBpclass, SpawnLocation, SpawnRotation, SpawnParams);
        //if (ItemLevel[i] < 2)
        NewItem->SpawnItemType = ItemLevel[i];

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

void ABattleGameMode::UpdateHp(Protocol::SC_UPDATE_HP_PKT& pkt)
{
    const uint16 playerId = pkt.player_id;
    if (SpawnedPlayers.Find(playerId) == nullptr)
        return;

    ANetworkCharacter* Player = Cast<ANetworkCharacter>(SpawnedPlayers[playerId]);
    if (Player == nullptr) return;
    Player->SetHP(pkt.hp);

    // TODO: 더 할게 있나?
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
        ABattle_PlayerController* PC = Cast<ABattle_PlayerController>(PlayerCharacter->GetController());
        if (PC && PC->PlayerHUD)
        if (CountSound)
        {
            UGameplayStatics::PlaySound2D(GetWorld(), CountSound);
        }
        
        // 플레이어의 HUD 업데이트
        PC->PlayerHUD->UpdateCountdown(DisplayTime);

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

    if (AmIHost == false) return;
    if (CurrentRoundTime == TIME_OVER)
    {
        Protocol::CS_TIME_OVER_PKT timeOverPkt;

        SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(timeOverPkt);
        Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
    }
}

void ABattleGameMode::SpawnItemsInArea1()
{
//    //// 예시로 영역1에 10개의 아이템 스폰
//    //const int32 NumItemsToSpawn = 30;
//    //if (Area1SpawnPoints.Num() == 0)
//    //{
//    //    UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: No spawn points available."));
//    //    return;
//    //}
//    // 예시로 영역1에 10개의 아이템 스폰
//    const int32 NumItemsToSpawn = 20;
//    if (Area1SpawnPoints.Num() == 0)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: No spawn points available."));
//        return;
//    }
//
//    //// 만약 NumItemsToSpawn이 전체 스폰 포인트보다 많으면, 경고 로그 출력
//    //if (NumItemsToSpawn > Area1SpawnPoints.Num())
//    //{
//    //    UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: Not enough spawn points available. Reducing item count."));
//    //}
//
//    //// SpawnedItems 배열 초기화
//    //SpawnedItems.Empty();
//
//    //UWorld* World = GetWorld();
//    //if (!World)
//    //{
//    //    UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: World is null."));
//    //    return;
//    //}
//
//    //// 지역 spawn 포인트 배열 복사 및 셔플
//    //TArray<FVector> LocalSpawnPoints = Area1SpawnPoints;
//    //
//    //Test::Shuffle(LocalSpawnPoints);
//    //
//    //int32 ItemsToSpawn = FMath::Min(NumItemsToSpawn, LocalSpawnPoints.Num());
//
//    //for (int32 i = 0; i < ItemsToSpawn; i++)
//    //{
//    //    FVector SpawnLocation = LocalSpawnPoints[i];
//    //    FRotator SpawnRotation = FRotator::ZeroRotator;
//
//    //    FActorSpawnParameters SpawnParams;
//    //    // 필요에 따라 SpawnParams.Owner 또는 Instigator 설정
//
//    //    ABaseItem* NewItem = World->SpawnActor<ABaseItem>(PowerUpBpclass, SpawnLocation, SpawnRotation, SpawnParams);
//
//    //    if (NewItem)
//    //    {
//    //        SpawnedItems.Add(NewItem);
//    //        UE_LOG(LogTemp, Log, TEXT("SpawnItemsInArea1: Spawned item at %s"), *SpawnLocation.ToString());
//    //    }
//    //    else
//    //    {
//    //        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: Failed to spawn item at index %d"), i);
//    //    }
//    //}
//    for (int32 i = 0; i < ItemsToSpawn; i++)
//    {
//        FVector SpawnLocation = LocalSpawnPoints[i];
//        FRotator SpawnRotation = FRotator::ZeroRotator;
//        
//
//        AItem_Box_Base* NewItem = World->SpawnActor<AItem_Box_Base>(ItemBoxBpclass, SpawnLocation, SpawnRotation);
//        NewItem->SpawnItemType = 0;
//        
//        if (NewItem)
//        {
//            SpawnedItems.Add(NewItem);
//            UE_LOG(LogTemp, Log, TEXT("SpawnItemsInArea1: Spawned item at %s"), *SpawnLocation.ToString());
//        }
//        else
//        {
//            UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: Failed to spawn item at index %d"), i);
//        }
//    }
}

void ABattleGameMode::InitializeArea1SpawnPoints()
{
    // 90개

    // 기존 좌표 배열 초기화
    Area1SpawnPoints.Empty();

    // 예시로 몇 개의 하드코딩 좌표를 추가 
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

    AreaSpawnPoints.Add(Area1SpawnPoints);
    
    UE_LOG(LogTemp, Log, TEXT("InitializeRegion1SpawnPoints: Initialized %d spawn points."), Area1SpawnPoints.Num());
}

void ABattleGameMode::InitializeArea2SpawnPoints()
{
    // 50개

    Area2SpawnPoints.Empty();

    Area2SpawnPoints.Add(FVector(-6077.f, -927.f, 760.f));
    Area2SpawnPoints.Add(FVector(-5899.f, 5123.f, 760.f));
    Area2SpawnPoints.Add(FVector(-3920.f, 5265.f, 760.f));
    Area2SpawnPoints.Add(FVector(-4478.f, 369.f, 760.f));
    Area2SpawnPoints.Add(FVector(-3231.f, -1757.f, 760.f));
    Area2SpawnPoints.Add(FVector(-3142.f, -3502.f, 760.f));
    Area2SpawnPoints.Add(FVector(-6257.f, -364.f, 760.f));
    Area2SpawnPoints.Add(FVector(-3476.f, -5821.f, 760.f));
    Area2SpawnPoints.Add(FVector(-1765.f, -3831.f, 760.f));
    Area2SpawnPoints.Add(FVector(-983.f, -2645.f, 760.f));
    Area2SpawnPoints.Add(FVector(969.f, -4282.f, 760.f));
    Area2SpawnPoints.Add(FVector(-1219.f, -6058.f, 760.f));
    Area2SpawnPoints.Add(FVector(-3985.f, -6539.f, 760.f));
    Area2SpawnPoints.Add(FVector(-6207.f, -6474.f, 760.f));
    Area2SpawnPoints.Add(FVector(1946.f, -4259.f, 760.f));
    Area2SpawnPoints.Add(FVector(3020.f, -2534.f, 760.f));
    Area2SpawnPoints.Add(FVector(4430.f, -6146.f, 760.f));
    Area2SpawnPoints.Add(FVector(6063.f, -6193.f, 760.f));
    Area2SpawnPoints.Add(FVector(3964.f, -1000.f, 760.f));
    Area2SpawnPoints.Add(FVector(5653.f, -966.f, 760.f));
    Area2SpawnPoints.Add(FVector(5897.f, 1365.f, 760.f));
    Area2SpawnPoints.Add(FVector(3440.f, 1548.f, 760.f));
    Area2SpawnPoints.Add(FVector(5723.f, 2800.f, 760.f));
    Area2SpawnPoints.Add(FVector(5506.f, 5278.f, 760.f));
    Area2SpawnPoints.Add(FVector(4697.f, 6289.f, 760.f));
    Area2SpawnPoints.Add(FVector(2561.f, 5930.f, 760.f));
    Area2SpawnPoints.Add(FVector(3346.f, 3769.f, 760.f));
    Area2SpawnPoints.Add(FVector(1354.f, 3186.f, 760.f));
    Area2SpawnPoints.Add(FVector(727.f, 6154.f, 760.f));
    Area2SpawnPoints.Add(FVector(-1618.f, 5996.f, 760.f));
    Area2SpawnPoints.Add(FVector(-2164.f, 3548.f, 760.f));
    Area2SpawnPoints.Add(FVector(-3341.f, 6341.f, 760.f));
    Area2SpawnPoints.Add(FVector(-6276.f, 5901.f, 760.f));
    Area2SpawnPoints.Add(FVector(-5683.f, 4451.f, 760.f));
    Area2SpawnPoints.Add(FVector(-5132.f, 2946.f, 760.f));
    Area2SpawnPoints.Add(FVector(-3233.f, 2384.f, 760.f));
    Area2SpawnPoints.Add(FVector(-1063.f, 2580.f, 760.f));
    Area2SpawnPoints.Add(FVector(1627.f, 2849.f, 760.f));
    Area2SpawnPoints.Add(FVector(2579.f, 865.f, 760.f));
    Area2SpawnPoints.Add(FVector(2617.f, -2016.f, 760.f));
    Area2SpawnPoints.Add(FVector(940.f, -2670.f, 760.f));
    Area2SpawnPoints.Add(FVector(-1966.f, -3540.f, 760.f));
    Area2SpawnPoints.Add(FVector(6068.f, 1640.f, 760.f));
    Area2SpawnPoints.Add(FVector(6672.f, 5137.f, 760.f));
    Area2SpawnPoints.Add(FVector(-1193.f, -4795.f, 1172.f));
    Area2SpawnPoints.Add(FVector(-5025.f, -1134.f, 1172.f));
    Area2SpawnPoints.Add(FVector(-1610.f, -4933.f, 1172.f));
    Area2SpawnPoints.Add(FVector(4641.f, -253.f, 1172.f));


    AreaSpawnPoints.Add(Area2SpawnPoints);
}

void ABattleGameMode::InitializeArea3SpawnPoints()
{
    Area3SpawnPoints.Empty();

    Area3SpawnPoints.Add(FVector(-4914.f, 1825.f, 2294.f));
    Area3SpawnPoints.Add(FVector(-4276.f, 6075.f, 2297.f));
    Area3SpawnPoints.Add(FVector(4237.f, 2158.f, 2297.f));
    Area3SpawnPoints.Add(FVector(1315.f, -5571.f, 2297.f));
    Area3SpawnPoints.Add(FVector(-4627.f, -3477.f, 1768.f));
    //Area3SpawnPoints.Add(FVector(0.f, 0.f, 1443.f));
    Area3SpawnPoints.Add(FVector(624.f, 669.f, 197.f));
    Area3SpawnPoints.Add(FVector(-576.f, 380.f, 204.f));
    Area3SpawnPoints.Add(FVector(-271.f, -782.f, 204.f));
    Area3SpawnPoints.Add(FVector(-424.f, -183.f, 209.f));

    AreaSpawnPoints.Add(Area3SpawnPoints);
}

void ABattleGameMode::SpawnItemsInArea3(Protocol::SC_SPAWN_ITEM_PKT& pkt)
{
    uint16 ZoneIndex = pkt.zone_index;

    int32 SpawnIndex[10];
    int32 ItemLevel[10];

    const uint16 Count = pkt.item_count;
    for (int32 i = 0; i < Count; ++i)
    {
        SpawnIndex[i] = static_cast<int32>(pkt.spawn_index[i]);
        ItemLevel[i] = static_cast<int32>(pkt.item_level[i]);
    }

    //UE_LOG(LogTemp, Warning, TEXT("%d - %d"), SpawnIndex[9], ItemLevel[9]);

    if (AreaSpawnPoints[ZoneIndex].Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: No spawn points available."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: World is null."));
        return;
    }

    for (int32 i = 0; i < Count; ++i)
    {
        int32 index = SpawnIndex[i];
        FVector SpawnLocation = AreaSpawnPoints[ZoneIndex][index];
        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        // 필요에 따라 SpawnParams.Owner 또는 Instigator 설정

        AItem_Box_High* NewItem = World->SpawnActor<AItem_Box_High>(ItemBoxHighBpclass, SpawnLocation, SpawnRotation, SpawnParams);
        //if (ItemLevel[i] < 2)

        if (NewItem)
        {
            NewItem->SpawnItemType = ItemLevel[i];
            //SpawnedItems.Add(NewItem);
            UE_LOG(LogTemp, Log, TEXT("SpawnItemsInArea1: Spawned item at %s"), *SpawnLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SpawnItemsInArea1: Failed to spawn item at index %d"), i);
        }
    }

}
