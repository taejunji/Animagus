#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShockwaveActor.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class PROJECT_ANIMAGUS_API AShockwaveActor : public AActor
{
    GENERATED_BODY()
    
public:    
    AShockwaveActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 충격파 범위 (구 형태의 콜리전)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* ShockwaveCollision;

    // 시각적 충격파 효과 (Niagara Component)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNiagaraComponent* ShockwaveEffect;

    // 현재 확산 반경 (초기 값 0, 시간이 지나면서 증가)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shockwave")
    float CurrentRadius;
    
public: 
    // 확산 속도 (cm/s, 예: 1000cm/s = 10m/s)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float ExpansionSpeed;


    // 충격파의 최대 반경 (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float MaxRadius;

    // 충격파 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float Damage;

    // 넉백(밀어내는 힘)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shockwave")
    float KnockbackForce;

    // 스킬 발동한 플레이어 (충격파의 발신자)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shockwave")
    AActor* Shooter;

    // 이미 데미지 및 넉백을 적용한 액터들을 기록 (중복 적용 방지)
    UPROPERTY()
    TArray<AActor*> AffectedActors;

    // 충격파 효과를 한 번만 적용할 것인지 여부
    bool bHasAppliedEffect;

    // 충격파 적용 범위 내의 액터에게 효과를 적용하는 함수
    void ApplyShockwaveEffects();

public:
    // Setter for Shooter
    void SetShooter(AActor* InShooter);
};
