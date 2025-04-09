// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_DistanceCheck.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTDecorator_DistanceCheck : public UBTDecorator
{
	GENERATED_BODY()
	
public:
    UBTDecorator_DistanceCheck();

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector patrol_pos_key;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float reset_range = 100.f;
};
