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


    
    public:
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
    
    /** 투사체를 소멸시키는 함수 (타이머에서 호출) */
    UFUNCTION()
    void DestroySkill();
};
