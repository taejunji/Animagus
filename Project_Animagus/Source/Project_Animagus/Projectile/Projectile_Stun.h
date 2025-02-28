// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Projectile_Stun.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AProjectile_Stun : public AProjectileBase
{
	GENERATED_BODY()

public:
    AProjectile_Stun();

protected:
    /** 스턴 지속 시간 (초) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
    float StunDuration;

    // OnHit 함수 오버라이드: 충돌 시 데미지 및 스턴 효과 적용
    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                       UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                       const FHitResult& Hit) override;
    
};
