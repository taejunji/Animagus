#include "Projectile_change.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile_change::AProjectile_change()
{
    // 추가 초기화가 필요한 경우 여기에 작성함
}

void AProjectile_change::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || OtherActor == Shooter)
    {
        return;
    }

   
    ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(OtherActor);
    
    if (HitCharacter)
    {
        PlayHitSound(Hit.Location); 
        FVector ProjectileDir = ProjectileMovement
            ? ProjectileMovement->Velocity.GetSafeNormal()
            : GetActorForwardVector();

        const float OffsetDistance = 100.f;
        FVector HitLocation      = HitCharacter->GetActorLocation();
        FVector TeleportLocation = HitLocation - ProjectileDir * OffsetDistance;
        TeleportLocation.Z       = Shooter->GetActorLocation().Z;

        Shooter->TeleportTo(TeleportLocation, Shooter->GetActorRotation(), false, true);
        DestroySkill();
    }
    else
    {
        
        if (HitSound_noPlayer)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                HitSound_noPlayer,
                GetActorLocation(),
                FRotator::ZeroRotator,
                1.f, 1.f, 0.f,
                AttenuationSettings
            );
        }

        UE_LOG(LogTemp, Log, TEXT("AProjectile_change: 캐릭터 외 충돌, 기본 OnHit 호출"));
        AProjectileBase::OnHit(OverlappedComponent, OtherActor, OtherComp, NormalImpulse, Hit);
    }
   
}


