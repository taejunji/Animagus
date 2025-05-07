// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ShouldRetreat.generated.h"

/**
 * 타겟과 체력을 비교해서 공격 스킬 위주로 쓸지, 방어 스킬 위주로쓸지 정하는 노드
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTDecorator_ShouldRetreat : public UBTDecorator
{
	GENERATED_BODY()
	
public:
    UBTDecorator_ShouldRetreat();  

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

    UPROPERTY(EditAnywhere, Category = "Blackboard") 
    FBlackboardKeySelector target_key; 
};