// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Defense.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EDefenseSkill : uint8
{
    Shield UMETA(DisplayName = "ShieldSkill")
};

UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_Defense : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTaskNode_Defense();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector target_key;

    // 쉴드 가능한지 확인 -> 타겟이 존재한다면
    bool CheckShieldSkill(UBehaviorTreeComponent& OwnerComp, class ABaseCharacter* Character);

};
