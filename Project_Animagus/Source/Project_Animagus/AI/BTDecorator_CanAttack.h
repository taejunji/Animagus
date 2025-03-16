// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanAttack.generated.h"

/**
 *  공격 사정거리 안에 있는지 검사해서 Bool 리턴
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTDecorator_CanAttack : public UBTDecorator
{
	GENERATED_BODY()
	
public:
    UBTDecorator_CanAttack();

protected:
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float can_attack_range = 200.f;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector target_key;
};
