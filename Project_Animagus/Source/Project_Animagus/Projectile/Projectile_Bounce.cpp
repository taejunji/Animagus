// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Bounce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile_Bounce::AProjectile_Bounce()
{

    // Bounce 관련 기본값 설정
    BounceCount = 0;
    MaxBounceCount = 5;
    
    // ProjectileMovementComponent가 존재한다면 Bounce 관련 설정 적용
    if (ProjectileMovement)
    {
        ProjectileMovement->bShouldBounce = true;
        ProjectileMovement->Bounciness = 0.6f; // 튕김 정도 (0~1)
        ProjectileMovement->Friction = 0.2f;
        ProjectileMovement->BounceVelocityStopSimulatingThreshold = 200.f; // 속도가 이 값 이하이면 튕김 멈춤
    }
}

void AProjectile_Bounce::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                const FHitResult& Hit)
{

    if (OtherActor == Shooter )
    {
        return;

    }
    
    // 충돌 대상이 유효하고, 자기 자신 및 발사자(Shooter)는 제외합니다.
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {
        // 만약 충돌 대상이 ACharacter라면 데미지 적용 후 히트 이펙트 재생, 소멸 처리
        if (ACharacter* HitCharacter = Cast<ACharacter>(OtherActor))
        {
            UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, nullptr);
            if (HitEffect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.Location, Hit.Normal.Rotation());
            }
            if (ProjectileLight)
            {
                ProjectileLight->SetIntensity(0.0f);
            }
            
                // 충돌 표면의 법선이 대상에게 밀리는 방향이므로 이를 이용
                FVector ImpulseDirection = -Hit.Normal;
                // 필요한 경우, 추가 보정: 예를 들어 대상의 뒤쪽으로 밀리게 하려면 -Hit.Normal을 사용할 수도 있음.
                // 여기서는 충돌 표면의 외부 방향으로 밀어내는 효과를 줍니다.
                FVector LaunchVelocity = ImpulseDirection * KnockbackForce;
                HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
            
            DestroySkill();
        }
        else
        {
            // 캐릭터가 아닌 경우 (맵 데이터 등)에는 BounceEffect를 스폰하고
            // ProjectileMovementComponent의 Bounce 동작에 맡깁니다.
            if (BounceEffect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BounceEffect, Hit.Location, Hit.Normal.Rotation());
            }
            
            ++BounceCount;
            if (BounceCount >= MaxBounceCount)
            {
                if (HitEffect)
                {
                    UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.Location, Hit.Normal.Rotation());
                }
                if (ProjectileLight)
                {
                    ProjectileLight->SetIntensity(0.0f);
                }
                
                DestroySkill();
            }
            // 여기서 다른 처리는 하지 않고, ProjectileMovementComponent가 반사 동작을 수행하도록 합니다.
        }
    }
}


