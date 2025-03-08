// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearBlackboardValue.generated.h"

/**
 *  UBTTask_BlackboardBase는 BB에 있는 정보를 갖다 쓸 수 있는 클래스
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTTask_ClearBlackboardValue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
    UBTTask_ClearBlackboardValue();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
