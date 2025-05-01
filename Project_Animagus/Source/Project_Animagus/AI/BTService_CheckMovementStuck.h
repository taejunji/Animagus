// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckMovementStuck.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTService_CheckMovementStuck : public UBTService
{
	GENERATED_BODY()
	
public:
    UBTService_CheckMovementStuck();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    FVector InitialLocation;
    float ElapsedTime;
    bool bInitialized;

    UPROPERTY(EditAnywhere, Category = "Movement Check")
    float TimeThreshold = 1.f; // 정체로 판단할 시간 (초)

    UPROPERTY(EditAnywhere, Category = "Movement Check")
    float DistanceThreshold = 100.0f; // 이동량 허용 거리 (cm)

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector bMoveStuckKey;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector can_set_target_key;
};
