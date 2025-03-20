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
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {
        ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(OtherActor);
        if (HitCharacter)
        {
            if (HitCharacter->GetIsDead()) return;

            // 발사자(Shooter)가 있으면 두 캐릭터의 위치를 교환
            if (Shooter)
            {
                FVector ShooterLocation = Shooter->GetActorLocation();
                FVector HitLocation = HitCharacter->GetActorLocation();
                bool bShooterTeleported = Shooter->TeleportTo(HitLocation, Shooter->GetActorRotation(), false, true);
                bool bHitTeleported = HitCharacter->TeleportTo(ShooterLocation, HitCharacter->GetActorRotation(), false, true);
                UE_LOG(LogTemp, Log, TEXT("AProjectile_change: 위치 교환 - Shooter Teleported: %d, HitCharacter Teleported: %d"), bShooterTeleported, bHitTeleported);
            }
            else
            {
                // 발사자 정보가 없으면 충돌한 캐릭터를 월드 원점으로 이동
                bool bTeleported = HitCharacter->TeleportTo(FVector::ZeroVector, HitCharacter->GetActorRotation(), false, true);
                UE_LOG(LogTemp, Log, TEXT("AProjectile_change: Shooter 없음 - %s를 원점으로 이동, Teleport 성공: %d"), *HitCharacter->GetName(), bTeleported);
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

