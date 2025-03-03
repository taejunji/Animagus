// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Projectile_change.generated.h"

/**
 * AProjectile_change
 * 투사체가 캐릭터와 충돌 시, 발사자와 충돌한 캐릭터의 위치를 교환하는 투사체 클래스.
 * Owner가 없는 경우, 충돌한 캐릭터를 월드 원점으로 이동시킴.
 */
UCLASS()
class PROJECT_ANIMAGUS_API AProjectile_change : public AProjectileBase
{
	GENERATED_BODY()
    
public:
    AProjectile_change();

protected:
    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                         UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                         const FHitResult& Hit) override;
    
};
