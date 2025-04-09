// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_DistanceCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyAIController.h"

UBTDecorator_DistanceCheck::UBTDecorator_DistanceCheck()
{
    NodeName = TEXT("Distance Check");
}

bool UBTDecorator_DistanceCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return false;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return false;

    AActor* AIPawn = AIController->GetPawn();
    if (!AIPawn) return false;

    FVector PatrolPos = BlackboardComp->GetValueAsVector(patrol_pos_key.SelectedKeyName);
    float Distance = FVector::Dist(AIPawn->GetActorLocation(), PatrolPos);

    return Distance <= reset_range; // reset_range 이하이면 도착한 것으로 판별하고 다시 reset
}
