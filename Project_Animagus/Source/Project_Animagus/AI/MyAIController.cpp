// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"

#include "Navigation/PathFollowingComponent.h"
#include "../Character/AICharacter.h"
#include "TimerManager.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"  // 시각 센서 추가
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AIPerceptionSystem.h"

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

            DefendRadiusKey.SelectedKeyName = FName(TEXT("DefendRadius"));
            BlackboardPtr->SetValueAsFloat(DefendRadiusKey.SelectedKeyName, 350.f);
        }

        if (AIPerceptionComponent) 
        {
            TargetKey.SelectedKeyName = FName(TEXT("Target"));
            BlackboardPtr->SetValueAsObject(TargetKey.SelectedKeyName, nullptr);

            AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AMyAIController::OnPerceptionUpdated);
        }
    }
#if 1
    StartBehaviorTree(); 
#endif

    // BB 에디터에 키가 추가되어야 한다.
    /*APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation() );
    GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());*/

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

    AAICharacter* AI = Cast<AAICharacter>(GetPawn());
    if (AI == nullptr) return;
    
    UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
    if (!BlackboardComponent) return;

    if (AI->GetIsDead())
    {
        // AI가 죽었으면 Behavior Tree를 멈춤
        if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
        {
            SetControlMode(AIControlMode::AIController);
            BehaviorTreeComponent->StopTree(); 
        }
    }

    // 타겟이 죽었는지 확인
    CheckAndDisableTargetIfDead();

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

    
    if (ControlMode != AIControlMode::AIController) return;

#if 0
    // [ 보이는지 확인해서 쫓아가서 쏘거나 마지막으로 발견한 위치에 가기 ] : BT 
    // 플레이어 시야에 들어오면 쳐다보기 
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    //
    if (LineOfSightTo(PlayerPawn))
    {
        // Actor 쳐다보고 따라오는거
    //    MoveToActor(PlayerPawn, AcceptanceRadius);
    //    SetFocus(PlayerPawn);
        SetFocalPoint(PlayerPawn->GetActorLocation() + FVector(0, 0, 50)); // 플레이어 중앙(50cm 위)를 바라보게 하기

        // 시야에 보이면 Plaeyr 따라가고 아니면 지우기 
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
        // 발사 방향 디버깅
    //    DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), PlayerPawn->GetActorLocation(), FColor::Blue, false, 2.0f, 0, 2.0f);
    }
    else
    {
        ClearFocus(EAIFocusPriority::Gameplay);
    //    StopMovement();

        // 해당 키의 값을 제거하고, 블랙보드에서 "존재하지 않는 상태"로 만든다.
        GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
    }
#endif

}

void AMyAIController::CheckAndDisableTargetIfDead()
{
    // Blackboard에서 TargetKey에 해당하는 타겟 객체를 가져옵니다
    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
    if (TargetCharacter == nullptr)
    {
        // 타겟이 없다면 아무것도 하지 않음
        return;
    }

    // 타겟이 죽었는지 확인
    if (TargetCharacter->GetIsDead())
    {
        // 포커스를 해제하는데, 현재 타겟과만 관련된 포커스 해제
        ClearFocus(EAIFocusPriority::Gameplay);  // Focus 해제
        GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);  // Blackboard에서 타겟 제거
    }
}

void AMyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    TSet<AActor*> NewlySensedActors; 

    FAIStimulus AIStimulus;

    for (AActor* UpdatedActor : UpdatedActors)
    {        
        // 시각 감지
        AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Sight);
        bool bSensed = AIStimulus.WasSuccessfullySensed();

        if (bSensed) 
        {
            NewlySensedActors.Add(UpdatedActor); 
            HandleSensedSight(UpdatedActor, bSensed, AIStimulus);
        }

        // 청각 감지
        AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Hearing);
        if (AIStimulus.WasSuccessfullySensed())
        {
            // HandleSensedHearing(AIStimulus.StimulusLocation);
        }

        // 피해 감지
        AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Damage);
        if (AIStimulus.WasSuccessfullySensed())
        {
            // HandleSensedDamage(UpdatedActor);
        }
    }

    // 기존에 감지된 액터 중에서 더 이상 감지되지 않는 액터 찾기
    for (AActor* PreviouslySensedActor : SensedActors)
    {
        if (!NewlySensedActors.Contains(PreviouslySensedActor))
        {
            HandleSensedSight(PreviouslySensedActor, false, AIStimulus);
        }
    }

    // 현재 감지된 액터 목록 업데이트
    SensedActors = NewlySensedActors;
}

FAIStimulus AMyAIController::CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense)
{
    // 특정 액터가 어떤 감각을 통해 감지되었는지 확인하는 함수입니다. 
    // 주어진 액터가 시각, 청각, 피해 등 특정 감각에 의해 감지되었는지 확인하고, 감지된 자극(Stimulus)을 반환

    FActorPerceptionBlueprintInfo ActorPerceptionBlueprintInfo;
	FAIStimulus ResultStimulus;

    // 액터의 인식 정보를 가져옵니다 특정 액터(Actor)에 대한 AI Perception 정보를 모두 가져오는 기능
    AIPerceptionComponent->GetActorsPerception(Actor, ActorPerceptionBlueprintInfo); 

	TSubclassOf<UAISense> QuerySenseClass;
    // 감각 종류에 따라 감지 클래스를 설정합니다.
	switch (AIPerceptionSense)
	{
	//case EAIPerceptionSense::EPS_None:
	//	break;
	case EAIPerceptionSense::EPS_Sight:
		QuerySenseClass = UAISense_Sight::StaticClass();
		break;
	//case EAIPerceptionSense::EPS_Hearing:
	//	QuerySenseClass = UAISense_Hearing::StaticClass();
	//	break;
	//case EAIPerceptionSense::EPS_Damage:
	//	QuerySenseClass = UAISense_Damage::StaticClass();
		break;
	//case EAIPerceptionSense::EPS_MAX:
	//	break;
	default:
		break;
	}

	TSubclassOf<UAISense> LastSensedStimulusClass;

    // 감지된 자극 리스트에서 해당 감각의 자극을 찾습니다.
	for (const FAIStimulus& AIStimulus : ActorPerceptionBlueprintInfo.LastSensedStimuli)
	{
		LastSensedStimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, AIStimulus);

        // 요청된 감각과 일치하는 자극을 찾으면 그 자극을 반환합니다.
		if (QuerySenseClass == LastSensedStimulusClass)
		{
			ResultStimulus = AIStimulus;
			return ResultStimulus;
		}

	}
	return ResultStimulus; // 감지되지 않으면 기본 자극 반환
}

void AMyAIController::HandleSensedSight(AActor* Actor, bool bSensed, FAIStimulus Stimulus)
{
    // 1. 시야에 들어올 때, 2. 시야에 안들어올 때

    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(Actor);
    if (TargetCharacter == nullptr) return;

    if (TargetCharacter->GetIsDead() || bSensed == false)
    {         
        ClearFocus(EAIFocusPriority::Gameplay); 
        GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName); 
    }
    else if (bSensed)
    {
        //SetFocalPoint(TargetCharacter->GetActorLocation() + FVector(0, 0, 50));
        //SetFocus(TargetCharacter);
        GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, TargetCharacter);
    }

    FString DebugMessage = FString::Printf(TEXT("Target %s"), bSensed ? TEXT("시야에 포착됐다") : TEXT("시야에 없어졌다"));
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, bSensed ? FColor::Purple : FColor::Red, DebugMessage);
}