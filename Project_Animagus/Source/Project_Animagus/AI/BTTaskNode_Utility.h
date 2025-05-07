// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Utility.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EUtilitySkill : uint8
{
    Haste UMETA(DisplayName = "Haste"),
    ChangeSkill UMETA(DisplayName = "ChangeSkill"),
    SmokeSkill UMETA(DisplayName = "SmokeSkill")
};

UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_Utility : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTaskNode_Utility();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
