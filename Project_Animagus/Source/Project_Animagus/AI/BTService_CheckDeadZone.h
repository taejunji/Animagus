// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckDeadZone.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTService_CheckDeadZone : public UBTService
{
	GENERATED_BODY()

public:
    UBTService_CheckDeadZone();
	
protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // 기준 중심 (XY 기준)
    FVector CenterXY;
    float AllowedXYRange;
    float AllowedZDifference;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector target_key;
};
