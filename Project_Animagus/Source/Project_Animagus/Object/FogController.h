// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FogController.generated.h"

class UNiagaraComponent;
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
    // 🔹 독 Fog의 크기 및 줄어드는 속도 설정
    UPROPERTY(EditAnywhere, Category = "Fog Settings")
    float InitialRadius = 15000.0f;

    UPROPERTY(EditAnywhere, Category = "Fog Settings")
    float MinRadius = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Fog Settings")
    float ShrinkRate = 100.0f;

    float CurrentRadius;

    // 🔹 충돌 감지를 위한 Sphere
    UPROPERTY(VisibleAnywhere, Category = "Collision")
    USphereComponent* FogCollision;

    // 🔹 나이아가라 독 Fog 효과
    UPROPERTY(EditAnywhere, Category = "Fog Effect")
    UNiagaraComponent* FogEffect;
    
};
