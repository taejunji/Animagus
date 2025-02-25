// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Projectile_MagicMissile.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AProjectile_MagicMissile : public AProjectileBase
{
    GENERATED_BODY()
    
public:
    AProjectile_MagicMissile();

protected:
    // 유도 기능이 발동되는 거리 (단위: cm, 30m = 3000cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
    float HomingActivationRadius;

    // 유도 가속도 (필요에 따라 조절)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Homing")
    float HomingAccelerationMagnitude;

    // Tick() 오버라이드하여 유도 대상 찾기
    virtual void Tick(float DeltaTime) override;
 
    // 충돌 시 데미지 적용 및 기타 효과 (오버라이드)
    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                       UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                       const FHitResult& Hit) override;
	
};
