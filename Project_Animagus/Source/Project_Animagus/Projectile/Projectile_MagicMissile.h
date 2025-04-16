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
    virtual void BeginPlay() override;

    // 가속도가 시작되는 시간 (초) - 이 시간이 지나면 갑자기 가속 시작
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acceleration")
    float AccelerationThreshold;

    // 가속 계수: 임계치 이후 매 Tick 당 배율 (예: 2.0이면 매 Tick마다 속도가 2배씩 증가하는 것은 아니고, 아주 빠른 증가를 유도)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acceleration")
    float SuddenAccelerationFactor;

    // 투사체가 생성된 시각 (BeginPlay()에서 설정)
    float SpawnTime;
    
    // Tick() 오버라이드: 매 프레임 가속도 적용
    virtual void Tick(float DeltaTime) override;
 
    // 충돌 시 데미지 적용 및 기타 효과 (오버라이드)
    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                       UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                       const FHitResult& Hit) override;;
	
};
