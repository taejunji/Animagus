// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Navigation/PathFollowingComponent.h"

#include "TimerManager.h"

AMyAIController::AMyAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true; // Tick 활성화

}

void AMyAIController::BeginPlay()
{
    Super::BeginPlay();

    // 시작하자마자 Behavior Tree 활성화 -> 레벨에 배치하자마자 실행됨
#if 1
    if (AIBehavior != nullptr)
    {
        ControlMode = AIControlMode::BehaviorTree;
        RunBehaviorTree(AIBehavior);

        // BB 에디터에 키가 추가되어야 한다.
        /*APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation() );
        GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());*/

    }

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
#endif
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