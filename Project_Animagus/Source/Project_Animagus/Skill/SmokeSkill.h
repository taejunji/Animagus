// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "SmokeSkill.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API USmokeSkill : public UBaseSkill
{
	GENERATED_BODY()

public:
    USmokeSkill();

    // 연막 투사체 블루프린트 클래스 (에디터에서 할당)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SmokeSkill|Projectile")
    TSubclassOf<class AProjectile_Smoke> SmokeProjectileBPClass;

    // 연막 투사체의 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SmokeSkill|Projectile")
    float SmokeProjectileSpeed;

    // ActiveSkill 함수 오버라이드: 스킬 사용 시 연막 투사체를 스폰함
    virtual void ActiveSkill_Implementation() override;
};
