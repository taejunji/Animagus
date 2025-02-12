// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShrinkingZone.generated.h"

UCLASS()
class PROJECT_ANIMAGUS_API AShrinkingZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShrinkingZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    // 축소 속도 (1m/s)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
    float ShrinkSpeed;

    // 초기 반지름 (150M)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
    float InitialRadius;

    // 현재 반지름
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
    float CurrentRadius;

    // Cylinder Collision Component (안전 구역 체크용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone")
    class UCapsuleComponent* CollisionComp;

    // Niagara System Component (독안개 시각 효과용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone")
    class UNiagaraComponent* NiagaraComp;

    // 플레이어에게 데미지를 적용하는 함수
    void ApplyGasDamage(AActor* AffectedActor, float DeltaTime);

    // 플레이어가 안전 구역 내에 있는지 판단하는 함수
    bool IsActorInsideZone(AActor* OtherActor) const;

    // 디버그용
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool bShowDebug = true;
};
