// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "ShockwaveSkill.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECT_ANIMAGUS_API UShockwaveSkill : public UBaseSkill
{
	GENERATED_BODY()
public:
    UShockwaveSkill();

    /** 충격파 액터 블루프린트 클래스 (에디터에서 할당) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    TSubclassOf<class AShockwaveActor> ShockwaveActorBPClass;

    /** 충격파 데미지 값 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float ShockwaveDamage;

    /** 충격파 넉백 힘 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float KnockbackForce;

    /** 충격파 확산 속도 (cm/s) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float ExpansionSpeed;

    /** 충격파 최대 반지름 (cm) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float MaxRadius;

    // ActiveSkill 인터페이스 오버라이드: 스킬 사용 시 호출됨
    virtual void ActiveSkill_Implementation() override;
};
