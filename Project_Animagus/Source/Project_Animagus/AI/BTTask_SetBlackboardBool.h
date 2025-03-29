// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetBlackboardBool.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTTask_SetBlackboardBool : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
    UBTTask_SetBlackboardBool();

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    bool bValueToSet; // True 또는 False 값을 설정할 변수

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
