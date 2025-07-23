// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DeathPowerUpItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API ADeathPowerUpItem : public ABaseItem
{
	GENERATED_BODY()
public:
    ADeathPowerUpItem();

    // 사망 캐릭터의 강화 횟수 저장
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="PowerUp")
    int32 StoredPowerUpCount;

    // 딜레이 후 충돌 활성화용 타이머
    FTimerHandle EnableTimerHandle;

    // 1.5초 뒤 재생할 나이아가라 이펙트
    UPROPERTY(EditDefaultsOnly, Category="Effects")
    UNiagaraSystem* DelayActivateEffect;

    // 충돌 컴포넌트 활성화
    void EnableCollisionAndEffect();

protected:
    virtual void BeginPlay() override;

    // 충돌 오버랩 처리
    virtual void OnItemOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
