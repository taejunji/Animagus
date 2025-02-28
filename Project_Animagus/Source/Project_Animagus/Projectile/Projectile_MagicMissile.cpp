#include "Projectile_MagicMissile.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Components/PointLightComponent.h"

AProjectile_MagicMissile::AProjectile_MagicMissile()
{
    PrimaryActorTick.bCanEverTick = true;

    // 유도 기능 기본값 설정
    HomingActivationRadius = 3000.0f;          // 30m
    HomingAccelerationMagnitude = 5000.f;       // 필요에 따라 조절

    // ProjectileMovement 컴포넌트가 있다면 초기에는 유도 기능 off로 설정
    if (ProjectileMovement)
    {
        ProjectileMovement->bIsHomingProjectile = false;
        ProjectileMovement->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
    }
}

void AProjectile_MagicMissile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 아직 유도 기능이 활성화되지 않았다면, 조건 검사
    if (ProjectileMovement && !ProjectileMovement->bIsHomingProjectile)
    {
        // 모든 ACharacter를 대상으로 검사
        TArray<AActor*> PotentialTargets;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), PotentialTargets);

        AActor* BestTarget = nullptr;
        float ClosestDistSq = HomingActivationRadius * HomingActivationRadius;
        for (AActor* Target : PotentialTargets)
        {
            // 발사자(Shooter)는 제외
            if (Target == Shooter) continue;

            float DistSq = FVector::DistSquared(Target->GetActorLocation(), GetActorLocation());
            if (DistSq <= ClosestDistSq)
            {
                BestTarget = Target;
                ClosestDistSq = DistSq;
            }
        }

        if (BestTarget)
        {
            // 유도 기능 활성화: 해당 대상의 루트 컴포넌트를 HomingTargetComponent로 지정
            ProjectileMovement->bIsHomingProjectile = true;
            ProjectileMovement->HomingTargetComponent = BestTarget->GetRootComponent();
            UE_LOG(LogTemp, Log, TEXT("MagicMissile homing activated on target: %s"), *BestTarget->GetName());
        }
    }
}

void AProjectile_MagicMissile::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                      const FHitResult& Hit)
{


    
    if (OtherActor == Shooter )
    {
        ProjectileLight->SetIntensity(0.0f);
        DestroySkill();
        
    }
    
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {
        // 적용할 데미지: UGameplayStatics::ApplyDamage
        UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, nullptr);

        // PointLight 강도 0으로 감소
        if (ProjectileLight)
        {
            ProjectileLight->SetIntensity(0.0f);
        }

        if (ACharacter* HitCharacter = Cast<ACharacter>(OtherActor))
        {
            // 충돌 표면의 법선이 대상에게 밀리는 방향이므로 이를 이용
            FVector ImpulseDirection = -Hit.Normal;
            // 필요한 경우, 추가 보정: 예를 들어 대상의 뒤쪽으로 밀리게 하려면 -Hit.Normal을 사용할 수도 있음.
            // 여기서는 충돌 표면의 외부 방향으로 밀어내는 효과를 줍니다.
            FVector LaunchVelocity = ImpulseDirection * KnockbackForce;
            HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
            
        }
        
        
        //  추가 이펙트: 예를 들어 HitEffect를 스폰
        if (HitEffect)
        {
            FRotator Rotation = Hit.Normal.Rotation();
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.Location, Rotation);
        }

        // 즉시 투사체 소멸
        DestroySkill();
    }
}


