#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
class UPointLightComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class PROJECT_ANIMAGUS_API AProjectileBase : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    AProjectileBase();
    
protected:

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    /** Collision sphere: Root component and collision handling */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USphereComponent* CollisionSphere;

    /** Point light component for visual effect */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UPointLightComponent* ProjectileLight;



    /** Projectile lifetime in seconds */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float Lifetime;

    
    /** 충돌 이벤트 처리 함수 */
    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                       UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                       const FHitResult& Hit);

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                        bool bFromSweep, const FHitResult & SweepResult);
    
    /** 충돌 시 상대 플레이어에게 적용할 Knockback Force (충돌 방향 반대 방향으로 밀어내는 힘) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Knockback")
    float KnockbackForce;


    
public:
    /** 타이머 핸들->LifeTime 시간을 계산해서 카메라 쉐이크 적용 안되도록 함 */
    struct FTimerHandle LifetimeHandle;
    
    // ** 카메라 쉐이크
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    TSubclassOf<class UCameraShakeBase> HitCameraShakeClass;

    /**투사체 생존 시간이 Limt을 넘어가면 쉐이크 안되도록 설정->거리가 멀어진 투사체는 카메라 흔들림 없도록 */ 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float CameraShakeDurationLimit;
    
    /** 플레이어와 발사체 간의 최대 거리를 제한해서 멀리 존재하는 투사체는 플레이어 카메라에 영향 안받도록*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float MaxShakeDistance;


    /** Shooter actor (투사체를 발사한 플레이어) */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Projectile")
    AActor* Shooter;
    
    /** Niagara system for spawn flash effect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* FlashEffect;

    /** Niagara system for hit effect (충돌 시) */
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* HitEffect;
    
    /** Damage value to apply on hit */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float DamageValue;

    /** Projectile movement component */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;

    /** Niagara component for continuous projectile effect */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UNiagaraComponent* ProjectileEffectComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float EffectScale;
    
    /** 투사체를 소멸시키는 함수 (타이머에서 호출) */
    UFUNCTION()
    void DestroySkill();
};
