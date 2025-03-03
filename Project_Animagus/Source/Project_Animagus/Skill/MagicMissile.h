// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "MagicMissile.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECT_ANIMAGUS_API UMagicMissile : public UBaseSkill
{
	GENERATED_BODY()

public:
    UMagicMissile();

    /** 매직 미사일 데미지 값 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MagicMissile|Damage")
    float MissileDamage;

    /** 매직 미사일 이동 속도 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MagicMissile|Movement")
    float MissileSpeed;

    /** 유도 기능 발동 거리 (cm) - 예: 3000cm = 30m */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MagicMissile|Homing")
    float HomingActivationRadius;

    /** 유도 가속도 (HomingAccelerationMagnitude) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MagicMissile|Homing")
    float HomingAccelerationMagnitude;

    /** 스폰할 매직 미사일 투사체 블루프린트 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MagicMissile")
    TSubclassOf<class AProjectile_MagicMissile> ProjectileBPClass;

    // ActiveSkill 인터페이스 오버라이드: 스킬 사용 시 호출
    virtual void ActiveSkill_Implementation() override;
    
};
