// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Attack.generated.h"

/**
 * 
 UENUM은 Unreal Engine에서 **열거형(enum)**을 정의할 때 사용하는 매크로
 BlueprintType은 이 열거형이 블루프린트에서 사용될 수 있게 만든다는 의미입니다. 
 BlueprintType을 지정하면 이 열거형을 블루프린트에서 사용할 수 있게 됩니다. 
 따라서, C++에서 정의된 enum을 블루프린트에서 드래그 앤 드롭 방식으로 사용할 수 있게 됩니다.
 uint8(8비트 부호 없는 정수) 타입을 사용하고 있습니다. 즉, 열거형 값이 0부터 255까지의 범위 내에서 저장

 */
UENUM(BlueprintType)
enum class EAttackSkill : uint8
{
    Fireball UMETA(DisplayName = "Fireball"),
    MagicMissile UMETA(DisplayName = "MagicMissile"), 
    RadialSkill UMETA(DisplayName = "RadialSkill"), 
    ShockwaveSkill UMETA(DisplayName = "ShockwaveSkill"), 
    Stun UMETA(DisplayName = "Stun") 
}; 

UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTaskNode_Attack();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector target_key; 

    // 쇼크 웨이브 가능한지 확인 -> 적과 가까운 경우에 사용
    bool CheckShockwaveSkill(UBehaviorTreeComponent& OwnerComp, class ABaseCharacter* Character);
};
