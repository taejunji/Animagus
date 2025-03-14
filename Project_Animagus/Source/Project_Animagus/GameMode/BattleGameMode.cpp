// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "../System//MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "../Character/BaseCharacter.h"
#include "../PlayerController/Battle_PlayerController.h"
#include "../AI/MyAIController.h"
#include "../Character/AICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
    spawn_transform.Add(0, FTransform(FRotator(0,  0, 0), FVector(-13500.0f, 0.0f, 800.f))); // Spawn1 
    spawn_transform.Add(1, FTransform(FRotator(0, 90, 0), FVector(0.0f, -13500.0f, 800.f))); // Spawn2
    spawn_transform.Add(2, FTransform(FRotator(0, 180, 0), FVector(13500.0f, 0.0f, 800.f))); // Spawn3 
    spawn_transform.Add(3, FTransform(FRotator(0, 270, 0), FVector(0.0f, 13500.0f, 800.f))); // Spawn4 

}

void ABattleGameMode::StartPlay()
{
    Super::StartPlay();

    InitBattleMode();
}

void ABattleGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
 
}

void ABattleGameMode::InitBattleMode()
{
    elasped_time = 0.0f;
    GetWorld()->GetTimerManager().ClearTimer(battle_timer_handle); // 타이머가 중지됨

    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));  
    if (MyGameInstance)
    {
        // 1초마다 경과시간 호출 함수 타이머 설정
        GetWorld()->GetTimerManager().SetTimer(battle_timer_handle, this, &ABattleGameMode::PrintElapsedtime, 1.0f, true); 

        // 플레이어 스폰
        SpawnPlayers();

        // 5초 후에 플레이어 입력 활성화
        FTimerHandle GameStartTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this, &ABattleGameMode::ActivateInput, start_time, false);
    }
}

void ABattleGameMode::SpawnPlayers()
{
    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MyGameInstance == nullptr) return;

    // Default Pawn 위치를 스폰 로케이션으로 설정하고 입력 비활성화
    ABattle_PlayerController* PlayerController = Cast<ABattle_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); // 첫 번째 플레이어 컨트롤러 가져오기
    if (PlayerController == nullptr) return;
    
    if (auto* Player = Cast<ABaseCharacter>(PlayerController->GetPawn()))
    {
        // 스폰 위치, 회전 [ Player 번호 생기면 변경 ]
        FVector SpawnLocation = spawn_transform[Player->player_number].GetLocation();
        FRotator SpawnRotation = spawn_transform[Player->player_number].Rotator();

        // 기본 Pawn 위치 변경
        PlayerController->GetPawn()->SetActorLocation(SpawnLocation);
        PlayerController->GetPawn()->SetActorRotation(SpawnRotation);

        // 컨트롤러 뷰 회전 설정
        PlayerController->SetControlRotation(SpawnRotation);

        // 입력 비활성화
        PlayerController->DisableInput(PlayerController);

        if (UCharacterMovementComponent* CharacterMovement = Cast<UCharacterMovementComponent>(PlayerController->GetPawn()->GetMovementComponent()))
        {
            CharacterMovement->SetMovementMode(EMovementMode::MOVE_Falling); // 움직이지 않도록 설정 MOVE_None / MOVE_Falling
        }

        // "0"번 플레이어가 아닌 경우 AI 생성하지 않고 나가기
        if (Player->player_number != 0) return;
    }

    // ** AI를 추가할 경우 -> 0번 플레이어만 만들 것임 ** AI 플레이어 수 설정
    for (int32 i = 1; i < 4; ++i)
    {
        // AI 플레이어 생성 (임의의 `ABaseCharacter`로 가정)
        FVector AI_SpawnLocation = spawn_transform[i].GetLocation();
        FRotator AI_SpawnRotation = spawn_transform[i].Rotator();

        // AI 캐릭터 스폰
        AAICharacter* AIChar = GetWorld()->SpawnActor<AAICharacter>(AIPlayerClass, AI_SpawnLocation, AI_SpawnRotation); 
        if (!AIChar) continue; 

        //if (UCharacterMovementComponent* MovementComp = AIChar->GetCharacterMovement())
        //{
        //    MovementComp->SetMovementMode(EMovementMode::MOVE_Flying);
        //}

        // AI 컨트롤러 생성 및 연결
        AMyAIController* AICtrl = GetWorld()->SpawnActor<AMyAIController>(AIControllerClass, AI_SpawnLocation, AI_SpawnRotation); 
        if (AICtrl) 
        { 
            AICtrl->Possess(AIChar); 
            AICtrl->SetControlRotation(AI_SpawnRotation); 
            AICtrl->SetIgnoreMoveInput(true); 
            AICtrl->SetIgnoreLookInput(true); 
        }

        // 게임 인스턴스에 AI 캐릭터 저장
        MyGameInstance->AddAICharacter(AIChar); 
    }
}

void ABattleGameMode::ActivateInput()
{
    ABattle_PlayerController* PlayerController = Cast<ABattle_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); // 첫 번째 플레이어 컨트롤러 가져오기
    if (PlayerController)
    {
        PlayerController->EnableInput(PlayerController);

        UCharacterMovementComponent* CharacterMovement = Cast<UCharacterMovementComponent>(PlayerController->GetPawn()->GetMovementComponent());
        if (CharacterMovement)
        {
            CharacterMovement->SetMovementMode(EMovementMode::MOVE_Walking); // 다시 걷기 모드로 전환 
        }

        // "0"번 플레이어가 아닌 경우 AI 생성하지 않고 나가기
        auto* Player = Cast<ABaseCharacter>(PlayerController->GetPawn());
        if (Player->player_number != 0) return;
    }

    // **AI들의 Behavior Tree 실행**
    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!MyGameInstance) return;

    // **AI들의 Behavior Tree 실행**
    for (AAICharacter* AIChar : MyGameInstance->AIPlayers)
    {
        if (AIChar)
        {
            AMyAIController* AICtrl = Cast<AMyAIController>(AIChar->GetController());
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
