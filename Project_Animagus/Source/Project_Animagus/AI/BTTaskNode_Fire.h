// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Fire.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_Fire : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTaskNode_Fire();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
