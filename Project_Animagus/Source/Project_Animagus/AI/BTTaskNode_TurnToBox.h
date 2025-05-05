// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_TurnToBox.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_TurnToBox : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTaskNode_TurnToBox();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
    bool b_is_turnning = false;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector box_target_key;
};
