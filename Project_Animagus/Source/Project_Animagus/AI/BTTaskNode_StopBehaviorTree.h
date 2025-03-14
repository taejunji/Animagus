// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_StopBehaviorTree.generated.h"

/**
 *  BT를 중지
        AIController로 AI를 움직일 것임
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_StopBehaviorTree : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTaskNode_StopBehaviorTree();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
