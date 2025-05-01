// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldActor.generated.h"

class USoundBase;
class USoundAttenuation;

UCLASS()
class PROJECT_ANIMAGUS_API AShieldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShieldActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 보호막 효과를 나타내는 충돌 컴포넌트 (예: Sphere)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
    class USphereComponent* ShieldCollision;

    // 보호막의 시각적 효과용 Niagara 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
    class UNiagaraComponent* ShieldEffect;

public:	
    // 보호막 지속 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
    float ShieldDuration;

    // 보호막이 소환된 플레이어(스킬 시전자)를 저장할 변수
    UPROPERTY(BlueprintReadOnly, Category = "Shield")
    AActor* ShieldOwner;

    UPROPERTY(EditAnywhere, Category="Audio")
    USoundBase* LaunchSound;
    
    UPROPERTY(EditAnywhere, Category="Audio")
    USoundBase* HitSound;

    UPROPERTY(EditAnywhere, Category="Audio")
    USoundAttenuation* AttenuationSettings;

    UFUNCTION()
    void OnShieldOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                              bool bFromSweep, const FHitResult & SweepResult);

public:
    // 보호막 액터 생성 시, 스킬 시전자의 정보를 저장하기 위한 함수
    void SetShieldOwner(AActor* NewOwner);

};
