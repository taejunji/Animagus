// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckShrinkingZone.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTService_CheckShrinkingZone : public UBTService
{
	GENERATED_BODY()

public:
    UBTService_CheckShrinkingZone();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector patrol_pos_key;

	
};
