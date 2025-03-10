// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_LookAtTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTService_LookAtTarget : public UBTService
{
	GENERATED_BODY()

public:
    UBTService_LookAtTarget();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector target_key;
	
};
