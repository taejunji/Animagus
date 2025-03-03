// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "Projectile_Bounce.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AProjectile_Bounce : public AProjectileBase
{
    GENERATED_BODY()
    
public:
    AProjectile_Bounce();

protected:
    /** Bounce 효과를 위한 Niagara 시스템 에셋 (옵션) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounce")
    class UNiagaraSystem* BounceEffect;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bounce")
    int32 BounceCount;

    /** 최대 Bounce 횟수 (예: 15번) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounce")
    int32 MaxBounceCount;
    
    // OnHit 함수 오버라이드: 캐릭터와 충돌 시 데미지 적용, 그 외의 경우 반사 효과 유지
    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                       UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                       const FHitResult& Hit) override;
	
};
