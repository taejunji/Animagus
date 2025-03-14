#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShrinkingZone.generated.h"

UCLASS()
class PROJECT_ANIMAGUS_API AShrinkingZone : public AActor
{
    GENERATED_BODY()
	
public:	
    AShrinkingZone();

protected:
    virtual void BeginPlay() override;

public:	
    virtual void Tick(float DeltaTime) override;

    // 축소 속도 (cm/s)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zone")
    float ShrinkSpeed;

    // 초기 LargeRadius 값 (예: 15000cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zone")
    float InitialRadius;

    // 현재 LargeRadius 값
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zone")
    float CurrentRadius;

    // Collision Component (안전구역 체크용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Zone")
    class UCapsuleComponent* CollisionComp;

    // Niagara System Component (시각 효과용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Zone")
    class UNiagaraComponent* NiagaraComp;

    // 플레이어에게 데미지를 적용하는 함수
    void ApplyGasDamage(AActor* AffectedActor, float DeltaTime);

    // 플레이어가 안전구역 내에 있는지 판단하는 함수
    bool IsActorInsideZone(AActor* OtherActor) const;

    // 디버그용: 안전구역 시각화 활성화 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
    bool bShowDebug = true;
};
