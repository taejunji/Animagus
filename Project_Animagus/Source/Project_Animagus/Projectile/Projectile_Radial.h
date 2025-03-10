// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Projectile_Radial.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AProjectile_Radial : public AProjectileBase
{
	GENERATED_BODY()

public:
    AProjectile_Radial();

protected:
    // BeginPlay()를 오버라이드해서 방사형 투사체
    virtual void BeginPlay() override;

    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                        const FHitResult& Hit) override;
    
};
