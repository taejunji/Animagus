// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogController.generated.h"

class UNiagaraComponent;
class UExponentialHeightFogComponent;
class USphereComponent;

UCLASS()
class PROJECT_ANIMAGUS_API AFogController : public AActor
{
    GENERATED_BODY()

public:    
    AFogController();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    
    // 🔹 스킬과 충돌 시 나이아가라 효과 생성
    void CreateEffectAtLocation(FVector ImpactLocation, FName SkillType);

    // 🔹 충돌 감지를 위한 함수
    UFUNCTION()
    void OnSkillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                        bool bFromSweep, const FHitResult& SweepResult);

    // 초기 반경 (300m → 15000cm)
    UPROPERTY(EditAnywhere, Category = "Fog Settings")
    float InitialRadius = 15000.0f;

    // 최소 반경 (맵 전체를 뒤덮을 때까지)
    UPROPERTY(EditAnywhere, Category = "Fog Settings")
    float MinRadius = 0.0f;

    // 독안개 줄어드는 속도 (1m/s = 100cm/s)
    UPROPERTY(EditAnywhere, Category = "Fog Settings")
    float ShrinkRate = 100.0f;

    // 현재 독안개 반경
    float CurrentRadius;

    // 독안개 업데이트 타이머
    FTimerHandle UpdateTimerHandle;

    // 🔹 충돌 감지를 위한 보이지 않는 Sphere
    UPROPERTY(VisibleAnywhere, Category = "Collision")
    USphereComponent* FogCollision;

    // 🔹 나이아가라 독안개 효과
    UPROPERTY(EditAnywhere, Category = "Fog Effect")
    UNiagaraComponent* FogEffect;

    // 🔹 볼류메트릭 포그
    UPROPERTY(EditAnywhere, Category = "Fog Effect")
    UExponentialHeightFogComponent* FogComponent;
    
};
