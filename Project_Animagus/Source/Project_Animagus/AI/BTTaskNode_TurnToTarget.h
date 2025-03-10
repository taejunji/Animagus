// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_TurnToTarget.generated.h"

/**
 *  Target을 향해 돌아보는 노드
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTaskNode_TurnToTarget();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    // TickTask()는 해당 BTTask 노드가 실행 중일 때 계속 호출되는 함수야.
    // 즉, ExecuteTask()에서 EBTNodeResult::InProgress를 반환하면, 이 노드가 실행되는 동안 TickTask()가 매 프레임 호출돼.

    // TickTask()가 언제 멈추는가?
    // 1. FinishLatentTask()를 호출할 때
    // 2. BT가 다른 노드를 실행할 때
private:
    bool b_is_turnning = false;

protected:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector target_key;

};
