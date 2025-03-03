// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "Stun.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UStun : public UBaseSkill
{
	GENERATED_BODY()
public:
    UStun();

    /** 스턴 투사체의 데미지 값 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stun|Damage")
    float StunDamage;

    /** 스턴 투사체의 이동 속도 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stun|Movement")
    float StunSpeed;

    /** 스턴 지속 시간 (초) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stun|Effect")
    float StunDuration;

    /** 스폰할 스턴 투사체 블루프린트 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stun")
    TSubclassOf<class AProjectile_Stun> ProjectileBPClass;

    // ActiveSkill 인터페이스 오버라이드: 스킬 사용 시 호출됨
    virtual void ActiveSkill_Implementation() override;
};
