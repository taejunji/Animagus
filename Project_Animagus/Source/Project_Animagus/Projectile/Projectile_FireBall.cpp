// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_FireBall.h"

#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Components/PointLightComponent.h"

AProjectile_FireBall::AProjectile_FireBall()
{
    // KnockbackForce 기본값 설정 (필요에 따라 조정)
    KnockbackForce = 3000.f;
}

void AProjectile_FireBall::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("Projectile_Fire_Ball BeginPlay: Location = %s"), *GetActorLocation().ToString());

    // FlashEffect가 할당되어 있다면 이름을 출력
    if (FlashEffect)
    {
        UE_LOG(LogTemp, Log, TEXT("Projectile_Fire_Ball: FlashEffect is assigned: %s"), *FlashEffect->GetName());
        // 한 번만 스폰되는 플래시 효과 실행 (필요에 따라, 지속 효과는 Niagara Component로 처리)
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FlashEffect, GetActorLocation(), GetActorRotation());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Projectile_Fire_Ball: FlashEffect is NOT assigned."));
    }

    // HitEffect에 대해서도 로그 (HitEffect는 충돌 시 스폰되므로 여기선 할당 여부만 확인)
    if (HitEffect)
    {
        UE_LOG(LogTemp, Log, TEXT("Projectile_Fire_Ball: HitEffect is assigned: %s"), *HitEffect->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Projectile_Fire_Ball: HitEffect is NOT assigned."));
    }

    // 만약 지속 효과용 Niagara 에셋(예: ContinuousEffect) 같은 것이 있다면, 그것도 로그에 추가합니다.
    // 예를 들어:
    // if (ContinuousEffect)
    // {
    //     UE_LOG(LogTemp, Log, TEXT("Projectile_Fire_Ball: ContinuousEffect is assigned: %s"), *ContinuousEffect->GetName());
    //     ProjectileEffectComponent->SetAsset(ContinuousEffect);
    //     ProjectileEffectComponent->Activate();
    // }
    // else
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("Projectile_Fire_Ball: ContinuousEffect is NOT assigned."));
    // }

    // 만약 ProjectileEffectComponent에 이미 할당된 에셋이 있다면 그것도 로그로 확인
    if (ProjectileEffectComponent)
    {
        if (ProjectileEffectComponent->GetAsset())
        {
            UE_LOG(LogTemp, Log, TEXT("Projectile_Fire_Ball: ProjectileEffectComponent asset: %s"), *ProjectileEffectComponent->GetAsset()->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Projectile_Fire_Ball: ProjectileEffectComponent has no asset assigned."));
        }
    }
}

void AProjectile_FireBall::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                 const FHitResult& Hit)
{
    // 기본 AProjectileBase의 충돌 처리 전에, 충돌 대상이 유효하고, 자신과 발사자(Shooter)와 충돌하지 않을 때 처리
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {
        // 충돌 지점의 법선 벡터를 회전값으로 변환 (충돌 표면의 외부 방향)
        FRotator Rotation = Hit.Normal.Rotation();

        // HitEffect 이펙트 스폰
        if (HitEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.Location, Rotation);
        }

        // PointLight 강도 0으로 감소
        if (ProjectileLight)
        {
            ProjectileLight->SetIntensity(0.0f);
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
        
        // else
        // {
        //     // 캐릭터 이외에도 넉백 넣고 싶으면 ㄱㄱ 
        //     if (OtherComp && OtherComp->IsSimulatingPhysics())
        //     {
        //         OtherComp->AddImpulseAtLocation(Hit.Normal * KnockbackForce, Hit.Location);
        //     }
        // }

        // (추후 화상 효과 적용 코드)

        // 충돌 후, 1초 후 DestroySkill 호출 )
        DestroySkill();
        
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
