// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Haste.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_Haste : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTaskNode_Haste();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
