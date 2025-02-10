// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AShrinkingZone.generated.h"

UCLASS()
class PROJECT_ANIMAGUS_API AAShrinkingZone : public AActor
{
    GENERATED_BODY()
	
public:    
    AAShrinkingZone();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;

    // 전체 맵의 반지름 (300M 원형이므로 150M)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShrinkingZone")
    float InitialSafeRadius = 150.0f;

    // 현재 안전구역 반지름
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShrinkingZone")
    float CurrentSafeRadius;

    // 레벨 시작 후 안전구역 축소 시작 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShrinkingZone")
    float ShrinkStartTime = 60.0f; // 1분 후

    // 축소 완료까지 걸리는 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShrinkingZone")
    float ShrinkDuration = 120.0f; // 2분

    // (선택 사항) 안전구역 외 플레이어에게 주는 피해량 (초당)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShrinkingZone")
    float DamagePerSecond = 10.0f;
    
private:
    // 레벨 시작 시각 저장
    float LevelStartTime;
    
    // 축소가 시작되었는지 여부
    bool bIsShrinking;
    
    // (선택 사항) 안전구역 외 플레이어에게 피해를 주는 함수 (플레이어 캐릭터 구조 미확정으로 주석 처리)
    void ApplyDamageOutsideSafeZone(float DeltaTime);

};
