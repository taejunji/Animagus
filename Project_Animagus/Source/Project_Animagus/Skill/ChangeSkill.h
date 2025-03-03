// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "ChangeSkill.generated.h"

/**
 * UChangeSkill
 * ChangeSkill 클래스.
 * 이 스킬을 사용하면 지정된 투사체(Projectile_change)를 스폰하여, 충돌 시 발사자와 충돌한 캐릭터의 위치를 교환하거나
 * 발사자가 없을 경우 충돌한 캐릭터를 월드 원점으로 이동시킨다.
 */


UCLASS(Blueprintable)
class PROJECT_ANIMAGUS_API UChangeSkill : public UBaseSkill
{
	GENERATED_BODY()
public:
    UChangeSkill();

    // 투사체에 적용할 데미지 (교환 효과에는 데미지가 없으면 0으로 설정 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Change|Damage")
    float ChangeDamage;

    // 투사체의 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Change|Movement")
    float ChangeSpeed;

    // 스폰할 투사체 블루프린트 클래스 (Projectile_change를 상속받은 클래스)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Change")
    TSubclassOf<class AProjectile_change> ProjectileBPClass;

protected:
    // 스킬 사용 시 호출되는 함수
    virtual void ActiveSkill_Implementation() override;
};
