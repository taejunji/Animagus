// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Projectile_Smoke.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AProjectile_Smoke : public AProjectileBase
{
	GENERATED_BODY()

public:
    AProjectile_Smoke();

protected:
    // 충돌 시 호출되는 함수. BaseProjectile의 OnHit()를 오버라이드.
    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                         UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                         const FHitResult& Hit) override;
};
