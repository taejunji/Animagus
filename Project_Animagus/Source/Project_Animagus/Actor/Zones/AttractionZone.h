// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraCommon.h"
#include "NiagaraDataInterfaceCollisionQuery.h"
#include "AttractionZone.generated.h"


UCLASS()
class PROJECT_ANIMAGUS_API AAttractionZone : public AActor
{
	GENERATED_BODY()
	
public:	
    AAttractionZone();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 박스 형태의 충돌 영역. 플레이어가 이 영역에 들어왔는지 판단.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttractionZone")
    class UBoxComponent* ZoneCollision;

    // Niagara 시스템 컴포넌트 (시각 효과용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttractionZone")
    class UNiagaraComponent* NiagaraComp;

    // 이 액터가 담당할 플레이어의 인덱스 (BattleGameMode의 SpawnedPlayers 배열 기준)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttractionZone")
    int32 ControlledPlayerIndex;

    // 영역 내에 있을 때 해당 플레이어에게 적용할 데미지 (초당)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttractionZone")
    float DamagePerSecond;

    // 매 틱마다 플레이어의 위치를 확인하고, Niagara 파라미터 업데이트 및 데미지 적용
    void UpdateAttractionParameters(float DeltaTime);

    // 영역 내에 있는지 판단 (Box Collision 사용)
    bool IsActorInsideZone(AActor* Actor) const;

    // 플레이어에게 데미지를 적용하는 함수
    void ApplyGasDamage(AActor* AffectedActor, float DeltaTime);
    
};
