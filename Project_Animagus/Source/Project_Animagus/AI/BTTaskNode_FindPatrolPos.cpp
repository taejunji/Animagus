// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FindPatrolPos.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "MyAIController.h"
#include "NavigationSystem.h"

UBTTaskNode_FindPatrolPos::UBTTaskNode_FindPatrolPos()
{
    NodeName = TEXT("Find Patrol Pos");
}

EBTNodeResult::Type UBTTaskNode_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (ControllingPawn == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    // 내비게이션 경로를 계산하는 시스템. GetRandomPointInNavigableRadius() 같은 기능 제공
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn);
    if (NavSystem == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    FNavLocation NextPatrol;
    // 1. OriginVector, Radius, ResultVector
    if (NavSystem->GetRandomPointInNavigableRadius(
        ControllingPawn->GetActorLocation(),
        search_radius,
        NextPatrol))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(patrol_pos_key.SelectedKeyName, NextPatrol);
        return EBTNodeResult::Succeeded;
    }


    return EBTNodeResult::Failed;
}
