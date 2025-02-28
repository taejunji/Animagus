// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "Bounce.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBounce : public UBaseSkill
{
	GENERATED_BODY()

public:
    UBounce();

    /** 반사 투사체 데미지 값 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bounce|Damage")
    float BounceDamage;

    /** 반사 투사체 이동 속도 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bounce|Movement")
    float BounceSpeed;

    /** 스폰할 반사 투사체 블루프린트 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bounce")
    TSubclassOf<class AProjectile_Bounce> ProjectileBPClass;

    // ActiveSkill 인터페이스 오버라이드: 스킬 사용 시 호출됩니다.
    virtual void ActiveSkill_Implementation() override;
	
};
