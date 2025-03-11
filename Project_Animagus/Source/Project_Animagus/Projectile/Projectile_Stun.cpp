// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Stun.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/PointLightComponent.h"

AProjectile_Stun::AProjectile_Stun()
{
    // 기본 스턴 지속 시간 (예: 2초)
    StunDuration = 2.0f;
}

void AProjectile_Stun::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                              const FHitResult& Hit)
{
    // 충돌 대상이 유효하고, 자신 및 발사자(Shooter)는 제외
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {
        // 대상이 캐릭터인 경우
        if (ACharacter* HitCharacter = Cast<ACharacter>(OtherActor))
        {
            // 데미지 적용
            UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, nullptr);
            
            // 스턴 효과 적용: 대상이 ABaseCharacter라면 ApplyStun()을 호출.
            if (ABaseCharacter* BaseChar = Cast<ABaseCharacter>(HitCharacter))
            {
                BaseChar->ApplyStun(StunDuration);
                if (BaseChar->GetIsHardHit() == false) { 
                    BaseChar->SetIsHardHit(true); 
                }

                UE_LOG(LogTemp, Log, TEXT("AProjectile_Stun: Applied stun to %s for %f seconds."), *OtherActor->GetName(), StunDuration);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("AProjectile_Stun: HitCharacter %s does not support stun."), *OtherActor->GetName());
            }
            
            // 히트 이펙트 재생 (설정되어 있다면)
            if (HitEffect)
            {
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.Location, Hit.Normal.Rotation());
            }

            // 충돌 시 투사체는 즉시 소멸
            DestroySkill();
        }
        else
        {
            // 캐릭터가 아닌 다른 오브젝트와 충돌 시, 기본 부모 클래스의 처리
           
            AProjectileBase::OnHit(OverlappedComponent, OtherActor, OtherComp, NormalImpulse, Hit);
        }
    }
}