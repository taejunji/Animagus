// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "NavigationSystem.h"

#include "Navigation/PathFollowingComponent.h"
#include "../Character/BaseCharacter.h"
#include "../Character/AICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../Skill/BaseSkill.h"
#include "TimerManager.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"  // 시각 센서 추가
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AIPerceptionSystem.h"

#include "../System/MyGameInstance.h"

#include "../Server/Server/protocol.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"


AMyAIController::AMyAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true; // Tick 활성화

    // BehaviorTree 애셋 로드
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/WorkFolder/AI/AIPlayer/BT_AIPlayer.BT_AIPlayer"));
    if (BTAsset.Succeeded()) 
    {
        AIBehavior = BTAsset.Object; 
    }

    // Blackboard 애셋 로드
    static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAsset(TEXT("/Game/WorkFolder/AI/AIPlayer/BB_AIPlayer.BB_AIPlayer"));
    if (BBAsset.Succeeded()) 
    {
        BlackboardData = BBAsset.Object;
    }

    // Perception Component 생성 및 초기화
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

}

void AMyAIController::BeginPlay()
{
    Super::BeginPlay();

    // 시작하자마자 Behavior Tree 활성화 -> 레벨에 배치하자마자 실행됨
    if (AIBehavior && BlackboardData)
    {
        UBlackboardComponent* BlackboardPtr = Blackboard.Get();

        if (UseBlackboard(BlackboardData, BlackboardPtr))
        {
            // 실제 Blackboard Asset에 IsRunning이라는 키가 있어야 한다.
            IsRunningKey.SelectedKeyName = FName(TEXT("IsRunning"));
            BlackboardPtr->SetValueAsBool(IsRunningKey.SelectedKeyName, true);

            // "AIState" 설정
            // AIStateKey.SelectedKeyName = FName(TEXT("AIState"));
            // BlackboardPtr->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EAIState::Patrol));

            can_set_target_key.SelectedKeyName = FName(TEXT("CanSetTarget"));
            BlackboardPtr->SetValueAsBool(can_set_target_key.SelectedKeyName, true);

            DefendRadiusKey.SelectedKeyName = FName(TEXT("DefendRadius"));
            BlackboardPtr->SetValueAsFloat(DefendRadiusKey.SelectedKeyName, 600.f); // 350

            AttackRadiusKey.SelectedKeyName = FName(TEXT("AttackRadius"));
            BlackboardPtr->SetValueAsFloat(AttackRadiusKey.SelectedKeyName, 600.f);

            Skill_isCoolTime_Key.SetNum(5);
            Skill_isCoolTime_Key[0].SelectedKeyName = FName(TEXT("Skill_0_Ready"));
            Skill_isCoolTime_Key[1].SelectedKeyName = FName(TEXT("Skill_1_Ready"));
            Skill_isCoolTime_Key[2].SelectedKeyName = FName(TEXT("Skill_2_Ready"));
            Skill_isCoolTime_Key[3].SelectedKeyName = FName(TEXT("Skill_3_Ready"));
            Skill_isCoolTime_Key[4].SelectedKeyName = FName(TEXT("Skill_4_Ready"));

            patrol_pos_key.SelectedKeyName = FName(TEXT("PatrolLocation"));
        }

        if (AIPerceptionComponent) 
        {
            TargetKey.SelectedKeyName = FName(TEXT("Target"));
            BlackboardPtr->SetValueAsObject(TargetKey.SelectedKeyName, nullptr);

            AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AMyAIController::OnPerceptionUpdated);
        }
    }
#if 0
    StartBehaviorTree(); 
#endif

    // -> 경로 출력
    //FAIMoveRequest MoveRequest;
    //MoveRequest.SetGoalLocation(FVector(0, 0, 0));
    //MoveRequest.SetAcceptanceRadius(15.f);
    //FNavPathSharedPtr NavPath;
    //MoveTo(MoveRequest, OUT& NavPath);
    //if (NavPath.IsValid())
    //{
    //    TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
    //    for (const auto& Point : PathPoints)
    //    {
    //        const FVector& Location = Point.Location;
    //        DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Red, false, 10.0f);
    //    }
    //}
}

void AMyAIController::StartBehaviorTree()
{
    if (AIBehavior != nullptr)
    {
        SetControlMode(AIControlMode::BehaviorTree);

        RunBehaviorTree(AIBehavior);
    }
}

void AMyAIController::SetControlMode(AIControlMode mode)
{
    switch (mode)
    {
    case AIControlMode::BehaviorTree:
        UE_LOG(LogTemp, Log, TEXT("Control Mode: BehaviorTree"));
        ControlMode = AIControlMode::BehaviorTree;
        break;

    case AIControlMode::AIController:
        UE_LOG(LogTemp, Log, TEXT("Control Mode: AIController"));
        ControlMode = AIControlMode::AIController;
        break;
    }
}

void AMyAIController::SetSkillCoolTime()
{
    UBlackboardComponent* BlackboardPtr = Blackboard.Get();

    if (auto* MyCharacter = Cast<ABaseCharacter>(GetPawn())) 
    {
        Skill_1_CoolTime_Key.SelectedKeyName = FName(TEXT("Skill_1_CoolTime")); 
        BlackboardPtr->SetValueAsFloat(Skill_1_CoolTime_Key.SelectedKeyName, MyCharacter->Skills[1]->CooldownTime);

        Skill_2_CoolTime_Key.SelectedKeyName = FName(TEXT("Skill_2_CoolTime"));
        BlackboardPtr->SetValueAsFloat(Skill_2_CoolTime_Key.SelectedKeyName, MyCharacter->Skills[2]->CooldownTime);

        Skill_3_CoolTime_Key.SelectedKeyName = FName(TEXT("Skill_3_CoolTime"));
        BlackboardPtr->SetValueAsFloat(Skill_3_CoolTime_Key.SelectedKeyName, MyCharacter->Skills[3]->CooldownTime);

        Skill_4_CoolTime_Key.SelectedKeyName = FName(TEXT("Skill_4_CoolTime"));
        BlackboardPtr->SetValueAsFloat(Skill_4_CoolTime_Key.SelectedKeyName, MyCharacter->Skills[4]->CooldownTime);
    }

}

void AMyAIController::ResumeBehaviorTree()
{
    if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent))
    {
        // Behavior Tree는 루트 노드부터 다시 시작합니다. 이때 Blackboard 값은 그대로 유지
        SetControlMode(AIControlMode::BehaviorTree);
        BTComp->RestartTree(EBTRestartMode::ForceReevaluateRootNode);
    }
}

void AMyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ABaseCharacter* AI = Cast<ABaseCharacter>(GetPawn());
    if (AI == nullptr) return;
    
    UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
    if (!BlackboardComponent) return;

    if (AI->GetIsDead()) return;

    CheckAndRecoverFromNavMesh();

    // 타겟이 죽었는지 확인
    CheckAndDisableTargetIfDead();

    // 스킬 쿨타임 확인
    for (int32 i = 0; i < AI->Skills.Num(); i++) 
    {
        if (AI->Skills.IsValidIndex(i) && AI->Skills[i] != nullptr)
        {
            BlackboardComponent->SetValueAsBool(Skill_isCoolTime_Key[i].SelectedKeyName, AI->Skills[i]->bIsCooldown);
        }
    }

    // Blackboard에서 키의 FKey를 가져옴
    FBlackboard::FKey IsRunningKeyID = BlackboardComponent->GetKeyID(IsRunningKey.SelectedKeyName); 

    bool bIsRunning = false;
    if (IsRunningKeyID != FBlackboard::InvalidKey) {
        bIsRunning = BlackboardComponent->GetValueAsBool(IsRunningKey.SelectedKeyName);
    }

    // 목표 속도 설정 (달리기 상태에 따라 달라짐)
    float TargetSpeed = bIsRunning ? AI->default_run_speed : AI->default_walk_speed;

    // 현재 속도를 목표 속도로 점진적으로 변경
    AI->current_speed = FMath::FInterpTo(AI->current_speed, TargetSpeed, DeltaTime, AI->speed_change_rate);

    // 캐릭터의 이동 속도 업데이트
    AI->SetWalkSpeed(AI->current_speed);


    // Send 판정
    bool ForceSendPacket = false;

    if (LastDesiredInput != DesiredInput)
    {
        ForceSendPacket = true;
        LastDesiredInput = DesiredInput;
    }

    MovePacketSendTimer -= DeltaTime;

    if (MovePacketSendTimer <= 0 || ForceSendPacket)
    {
        // State 설정

        if (AI->GetMovementComponent()->IsFalling() == false)
        {
            AI->SetMoveState(Protocol::PlayerState::MOVE_STATE_RUN);

            if (AI->GetVelocity().Size2D() <= 0.3f)
                AI->SetMoveState(Protocol::PlayerState::MOVE_STATE_IDLE);
        }

        MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

        Protocol::CS_AI_MOVE_PKT MovePkt;

        // 현재 위치 정보
        {
            Protocol::PlayerInfo Info;
            FVector Location = AI->GetActorLocation();
            Info.x = Location.X; Info.y = Location.Y; Info.z = Location.Z;
            Info.rotation = AI->GetActorRotation().Yaw;
            Info.player_id = AI->GetPlayerId();
            //Info.player_type = AI->GetPlayerType();
            Info.player_state = AI->GetMoveState();
            Info.speed_2d = AI->GetMovementComponent()->Velocity.Size2D();
            Info.speed_z = AI->GetMovementComponent()->Velocity.Z;

            MovePkt.player_info = Info;
        }

        SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(MovePkt);
        Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
    }

    if (ControlMode != AIControlMode::AIController) return;

}

void AMyAIController::CheckAndDisableTargetIfDead()
{
    // Blackboard에서 TargetKey에 해당하는 타겟 객체를 가져옵니다
    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));

    // 타겟이 죽었는지 확인
    if (TargetCharacter == nullptr || TargetCharacter->GetIsDead())
    {
        ClearFocusTarget();
    }
}

void AMyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    bool bCanSet = GetBlackboardComponent()->GetValueAsBool(can_set_target_key.SelectedKeyName);

    if (bCanSet == false) return; // 타겟 변경이 불가능한 경우 나가기

    TSet<AActor*> CandidateTargets; // 후보군 타겟들을 저장할 집합 (중복 방지) 
    FAIStimulus AIStimulus;

    for (AActor* UpdatedActor : UpdatedActors)
    {        
        // 시각 감지
        AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Sight);
        if (AIStimulus.WasSuccessfullySensed())
        {
            CandidateTargets.Add(UpdatedActor);
            // HandleSensedSight(UpdatedActor, true, AIStimulus);
        }

        // 청각 감지
        //AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Hearing);
        //if (AIStimulus.WasSuccessfullySensed())
        //{
        //    // HandleSensedHearing(AIStimulus.StimulusLocation);
        //}

        // 피해 감지
        AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Damage);
        if (AIStimulus.WasSuccessfullySensed())
        {
            CandidateTargets.Add(UpdatedActor);
        }
    }

    ABaseCharacter* BestTarget = SelectBestTarget(CandidateTargets); 

    SetAITarget(BestTarget); 
}

FAIStimulus AMyAIController::CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense)
{
    FActorPerceptionBlueprintInfo ActorPerceptionBlueprintInfo;
	FAIStimulus ResultStimulus;

    AIPerceptionComponent->GetActorsPerception(Actor, ActorPerceptionBlueprintInfo); 

	TSubclassOf<UAISense> QuerySenseClass;
	switch (AIPerceptionSense)
	{

	//case EAIPerceptionSense::EPS_None:
	//	break;

	case EAIPerceptionSense::EPS_Sight:
		QuerySenseClass = UAISense_Sight::StaticClass();
		break;

	case EAIPerceptionSense::EPS_Hearing:
		QuerySenseClass = UAISense_Hearing::StaticClass();
		break;

	case EAIPerceptionSense::EPS_Damage: 
		QuerySenseClass = UAISense_Damage::StaticClass(); 
		break;

	//case EAIPerceptionSense::EPS_MAX:
	//	break;

	default:
		break;
	}

	TSubclassOf<UAISense> LastSensedStimulusClass;

	for (const FAIStimulus& AIStimulus : ActorPerceptionBlueprintInfo.LastSensedStimuli)
	{
		LastSensedStimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, AIStimulus);

		if (QuerySenseClass == LastSensedStimulusClass)
		{
			ResultStimulus = AIStimulus;
			return ResultStimulus;
		}
	}
	return ResultStimulus;
}

void AMyAIController::HandleSensedSight(AActor* Actor, bool bSensed, FAIStimulus Stimulus)
{
    // 1. 시야에 들어올 때, 2. 시야에 안들어올 때

    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(Actor);
    if (TargetCharacter == nullptr) return;
     
    if (TargetCharacter->GetIsDead()) 
    {
        ClearFocusTarget();  
    }
    else if (bSensed)
    {
        GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, TargetCharacter);
    }
}

float AMyAIController::CalculateTargetPriority(ABaseCharacter* TargetCharacter) 
{
    const float DistanceWeight = 1.0f; 
    const float HPWeight = 0.5f; 

    if (!IsValid(GetPawn()) || !IsValid(TargetCharacter)) return FLT_MAX; // 기본 우선순위 반환 
    // 1. 가장 가까운 적 (거리 계산)
    float Distance = FVector::Dist(GetPawn()->GetActorLocation(), TargetCharacter->GetActorLocation());

    // 2. 자신을 때린 적 (혹은 해당 조건)
    //if (TargetCharacter->HasRecentlyAttacked(GetPawn()))
    //{
    //    Priority -= 50.0f; // 자신을 때린 적에게 우선순위 부여
    //}

    // 3. HP가 적은 적
    float HP = TargetCharacter->GetHP();

    return (Distance * DistanceWeight) + (HP * HPWeight); 
}

ABaseCharacter* AMyAIController::SelectBestTarget(const TSet<AActor*>& Candidates)
{
    ABaseCharacter* BestTarget = nullptr;
    float BestPriority = FLT_MAX;

    for (AActor* Candidate : Candidates)
    {
        ABaseCharacter* CandidateCharacter = Cast<ABaseCharacter>(Candidate);
        if (!CandidateCharacter) continue;

        float CurrentPriority = CalculateTargetPriority(CandidateCharacter);
        if (CurrentPriority < BestPriority)
        {
            BestPriority = CurrentPriority;
            BestTarget = CandidateCharacter;
        }
    }

    return BestTarget;
}

void AMyAIController::SetAITarget(ABaseCharacter* NewTarget)
{
    if (NewTarget)
    {
        // 중복 타이머 방지
        GetWorld()->GetTimerManager().ClearTimer(TargetChangeTimerHandle);

        GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, NewTarget);
        GetBlackboardComponent()->SetValueAsBool(can_set_target_key.SelectedKeyName, false);

        GetWorld()->GetTimerManager().SetTimer(TargetChangeTimerHandle, this, &AMyAIController::ResetTargetChange, 10.0f, false);
    }
    else
    {
        ClearFocusTarget();
    }
}

void AMyAIController::ResetTargetChange()
{
    GetBlackboardComponent()->SetValueAsBool(can_set_target_key.SelectedKeyName, true);
}

// 보완 필요
void AMyAIController::CheckAndRecoverFromNavMesh()
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (nullptr == NavSys) return;

    FVector CurrentLocation = GetPawn()->GetActorLocation();
    FNavLocation ProjectedLocation;

    // 월드상의 임의의 위치가 내비게이션 메시(NavMesh) 위에 있는지 확인하고, 가장 가까운 네비 메시 위의 위치를 반환
    bool bOnNavMesh = NavSys->ProjectPointToNavigation(CurrentLocation, ProjectedLocation, FVector(30.f, 30.f, 100.f));

    if (false == bOnNavMesh && false == bFailedToFindNavMesh)
    {
        FVector PatrolLoc = GetBlackboardComponent()->GetValueAsVector(patrol_pos_key.SelectedKeyName); 

        // 결과 위치에 디버그 구를 그립니다.
        // DrawDebugSphere(GetWorld(), PatrolLoc, 200.f, 12, FColor::Red, false, 0.5f);

        // 가까운 네비 위치 방향으로 걸어가게 명령
        FVector Direction = (PatrolLoc - CurrentLocation);
        Direction.Z = 0; // Z 제거
        Direction = Direction.GetSafeNormal();

        if (AAICharacter* AICharacter = Cast<AAICharacter>(GetPawn()))
        {
            AICharacter->AddMovementInput(Direction, 1.0f);
            AICharacter->JumpAI();
        }

        //GEngine->AddOnScreenDebugMessage(
        //    -1,
        //    2.0f,
        //    FColor::Red,
        //    FString::Printf(TEXT("Failed To Find NavMesh => True "))
        //);

        // 네비메시를 찾지 못한 상태를 기록
        bFailedToFindNavMesh = true;
    }
    else {
        // 디버그 구 그리기
        // DrawDebugSphere(GetWorld(), ProjectedLocation.Location, 200.f, 12, FColor::Blue, false, 0.5f);

        // 네비메시가 발견되면 다시 실패 토글을 리셋
        bFailedToFindNavMesh = false;
    }
}

void AMyAIController::ClearFocusTarget()
{
    if (ABaseCharacter* AI = Cast<ABaseCharacter>(GetPawn())) {
        AI->bUseControllerRotationYaw = false;
        AI->GetCharacterMovement()->bOrientRotationToMovement = true;
        AI->GetCharacterMovement()->bUseControllerDesiredRotation = false;
    }

    ClearFocus(EAIFocusPriority::Gameplay);  // Focus 해제
    GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);
    GetBlackboardComponent()->SetValueAsBool(can_set_target_key.SelectedKeyName, true);

}
