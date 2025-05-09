// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckTargetInSight.generated.h"

/**
 * "지금 보이는지 모르겠지만, 최근(MaxAge 이내)에 확실히 본 적이 있다"
→ 그 기억이 아직 유효하다면 true 리턴   
 * 
 * "최근 10초 안에 타겟이 시야에 감지된 적이 있고, 그 감지 정보가 아직 만료되지 않았다면 → 시야에 잡힌 것으로 간주한다."
 * 
    AI Perception Component가 기억하고 있는 모든 감지 기록(LastSensedStimuli)을 가져온다.
    그 중에서 시야(Sight) 감각으로 감지된 Stimulus만 확인한다.
    시야(Sight)로 감지됐고, 성공적으로 인식한 Stimulus가 있으면
    바로 true 반환해서 "Target이 시야에 보인다"고 판단한다.
    만약 시야로 감지된 기록이 없거나, 마지막으로 본 결과가 실패(안 보임)이면 false를 반환한다.
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBTDecorator_CheckTargetInSight : public UBTDecorator
{
	GENERATED_BODY()
	
public:
    UBTDecorator_CheckTargetInSight();

protected:
    // 데코레이터 조건 체크 함수
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
    // 시야각을 확인하는 함수
    bool IsTargetInSight(class AMyAIController* AIController, AActor* TargetActor) const;
};
