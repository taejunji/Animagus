// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Projectile_FireBall.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AProjectile_FireBall : public AProjectileBase
{
    GENERATED_BODY()
    
public:
    AProjectile_FireBall();

protected:
    // 화상 데미지 같은 기능은 추후 추가
    
    // (추후 화상 효과 관련 변수 추가 가능)
    
    // OnHit 함수 오버라이드: 기본 충돌 처리 외에 knockback 효과 적용
    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                       UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                       const FHitResult& Hit) override;
	
};
