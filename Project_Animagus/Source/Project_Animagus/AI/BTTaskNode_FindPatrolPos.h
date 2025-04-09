// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindPatrolPos.generated.h"

/**
 *  순찰 포지션 찾는 노드
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTaskNode_FindPatrolPos();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    // 이렇게 하면 에디터에서 블랙보드에서 추가한 키를 설정할 수 있고 
    // C++에서 OwnerComp.GetBlackboardComponent()->GetValueAsObject(target_key.SelectedKeyName) 이런식으로 가져올 수 있는 것
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector patrol_pos_key;

    //UPROPERTY(EditAnywhere, Category = "Blackboard")
    //float search_radius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector random_patrol_pos_key;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector random_patrol_radius_key;
};
