// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_SetPatrolOriginRadius.generated.h"

/**
 * 
 */

 // 거리 정보 저장용 구조체
struct FIndexDistancePair
{
    int32 Index;
    float DistanceSq;
};

// 정렬 기준 정의
struct FCompareDistance
{
    bool operator()(const FIndexDistancePair& A, const FIndexDistancePair& B) const
    {
        return A.DistanceSq < B.DistanceSq;
    }
};

UCLASS()
class PROJECT_ANIMAGUS_API UBTTaskNode_SetPatrolOriginRadius : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTaskNode_SetPatrolOriginRadius();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector random_patrol_pos_key;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector random_patrol_radius_key;
};
