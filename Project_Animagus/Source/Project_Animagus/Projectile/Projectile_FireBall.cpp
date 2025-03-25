// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_FireBall.h"

#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Project_Animagus/Actor/Shield/ShieldActor.h"


AProjectile_FireBall::AProjectile_FireBall()
{
    // KnockbackForce 기본값 설정 (필요에 따라 조정)
    KnockbackForce = 3000.f;
}


void AProjectile_FireBall::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                 const FHitResult& Hit)
{

    if (OtherActor == Shooter )
    {
        return;
       // ProjectileLight->SetIntensity(0.0f);
       //  DestroySkill();
        
    }
    
    AShieldActor* Shield = Cast<AShieldActor>(OtherActor);
    if (Shield && Shield->ShieldOwner && Shield->ShieldOwner == Shooter)
    {
        UE_LOG(LogTemp, Warning, TEXT("AProjectileBase: Ignoring hit with owner's shield."));
        CollisionSphere->IgnoreActorWhenMoving(OtherActor, true);
        return;
    }
    
    // 기본 AProjectileBase의 충돌 처리 전에, 충돌 대상이 유효하고, 자신과 발사자(Shooter)와 충돌하지 않을 때 처리
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {

        UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, nullptr);
        
        // 충돌 지점의 법선 벡터를 회전값으로 변환 (충돌 표면의 외부 방향)
      

        // HitEffect 이펙트 스폰
        if (HitEffect)
        {
            FRotator Rotation = Hit.Normal.Rotation();
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.Location, Rotation);
        }

        // PointLight 강도 0으로 감소
        if (ProjectileLight)
        {
            ProjectileLight->SetIntensity(0.0f);
        }

        // 카메라 쉐이크, FireBall은 생존시간이 길어지면 쉐이크 안되도록, 플레이어와 거리가 멀면 쉐이크 안되도록
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController(); 
        float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(LifetimeHandle); 

        if (PlayerController && HitCameraShakeClass && ElapsedTime < CameraShakeDurationLimit) 
        {
            if (ACharacter* Player = Cast<ACharacter>(PlayerController->GetPawn()))
            {
                // 플레이어와 투사체 거리 계산
                float DistanceToPlayer = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
                if (DistanceToPlayer < MaxShakeDistance) // ex) 10m 안에서만 쉐이크
                {
                    PlayerController->ClientStartCameraShake(HitCameraShakeClass);
                }
            }
        }

        // Knockback 효과 적용 (상대가 ACharacter인 경우)
        if (ACharacter* HitCharacter = Cast<ACharacter>(OtherActor))
        {
            // 충돌 표면의 법선이 대상에게 밀리는 방향이므로 이를 이용
            FVector ImpulseDirection = -Hit.Normal;
            // 필요한 경우, 추가 보정: 예를 들어 대상의 뒤쪽으로 밀리게 하려면 -Hit.Normal을 사용할 수도 있음.
            // 여기서는 충돌 표면의 외부 방향으로 밀어내는 효과를 줍니다.
            FVector LaunchVelocity = ImpulseDirection * KnockbackForce;
            HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
            
        }
        
        DestroySkill();

        
        // else
        // {
        //     // 캐릭터 이외에도 넉백 넣고 싶으면 ㄱㄱ 
        //     if (OtherComp && OtherComp->IsSimulatingPhysics())
        //     {
        //         OtherComp->AddImpulseAtLocation(Hit.Normal * KnockbackForce, Hit.Location);
        //     }
        // }

        // (추후 화상 효과 적용 코드)

        // 충돌 후, 1초 후 DestroySkill 호출
        // FTimerHandle DestroyHandle;
        // GetWorld()->GetTimerManager().SetTimer(
        //     DestroyHandle,
        //     this,
        //     &AProjectileBase::DestroySkill,
        //     1.0f,
        //     false
        // );
    }
}
