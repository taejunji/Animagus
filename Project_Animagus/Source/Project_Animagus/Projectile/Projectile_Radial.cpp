// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Radial.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/PointLightComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AProjectile_Radial::AProjectile_Radial()
{

}

void AProjectile_Radial::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("Projectile_Radial BeginPlay: %s"), *GetName());
    
 
}

void AProjectile_Radial::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor == Shooter )
    {
        ProjectileLight->SetIntensity(0.0f);
        DestroySkill();
        
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
    }
}
