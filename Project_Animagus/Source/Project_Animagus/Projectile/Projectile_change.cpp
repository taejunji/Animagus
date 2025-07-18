#include "Projectile_change.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"

AProjectile_change::AProjectile_change()
{
    // 추가 초기화가 필요한 경우 여기에 작성함
}

void AProjectile_change::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                const FHitResult& Hit)
{
    // 충돌한 대상이 유효하고, 자기 자신 및 발사자(Shooter)가 아닐 때 처리함
    if (OtherActor && OtherActor != this)// && OtherActor != Shooter)
    {
        
        ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(OtherActor);

        if (HitSound_noPlayer)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                HitSound_noPlayer,
                this->GetActorLocation(),
                FRotator::ZeroRotator,
                1.f, 1.f, 0.f,
                AttenuationSettings
            );
        }
        
        if (HitCharacter)
        {
            if (HitCharacter->GetIsDead()) return;
            PlayHitSound(Hit.Location);
            // 발사자(Shooter)가 있으면 두 캐릭터의 위치를 교환
            if (Shooter)
            {
                // FVector ShooterLocation = Shooter->GetActorLocation();
                FVector HitLocation = HitCharacter->GetActorLocation();
                Shooter->TeleportTo(HitLocation, Shooter->GetActorRotation(), false, true);
                // HitCharacter->TeleportTo(ShooterLocation, HitCharacter->GetActorRotation(), false, true);
                
            }
            else
            { 
                UE_LOG(LogTemp, Log, TEXT("AProjectile_change: Shooter 없음 - error"), *HitCharacter->GetName());
            }
            DestroySkill();
        }
        else
        {
            
            UE_LOG(LogTemp, Log, TEXT("AProjectile_change: 캐릭터가 아닌 객체와 충돌, 기본 OnHit 호출"));
            AProjectileBase::OnHit(OverlappedComponent, OtherActor, OtherComp, NormalImpulse, Hit);
        }
    }
}

