// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "HasteSkill.generated.h"

class ABaseCharacter;
class APlayerCharacter;
struct FTimerHandle;


/**
 * 
 */
UCLASS(Blueprintable)
class PROJECT_ANIMAGUS_API UHasteSkill : public UBaseSkill
{
    GENERATED_BODY()

public:
    UHasteSkill();

    /** 효과 지속 시간(초) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haste")
    float Duration;

    /** 적용할 달리기 속도 (default_run_speed을 이 값으로 변경) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haste")
    float BoostedRunSpeed;

    /** 포스트프로세싱 효과를 켜기 위한 속도 임계치 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haste")
    float PostProcessSpeedThreshold;
    
    /** 스킬 활성화 구현 */
    virtual void ActiveSkill_Implementation() override;

    /** 스킬 비활성화 구현 */
    virtual void DeactiveSkill_Implementation() override;

private:
    /** 발동 전 원래 달리기 속도 저장 */
    float OriginalRunSpeed;

    /** 비활성화 타이머 핸들 */
    FTimerHandle HasteTimerHandle;

    /** 속도 검사용 반복 타이머 핸들 */
    FTimerHandle SpeedCheckTimerHandle;

    /** 주기적으로 현재 속도를 검사해 PostProcess를 토글 */
    void CheckSpeed();
    
    /** 월드 내 모든 PostProcessVolume의 Blendables[index] 가중치 조절 */
    void SetHastePostProcess(float NewWeight);
};
