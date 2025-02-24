#include "Fireball.h"
#include "../Projectile/Projectile_FireBall.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"


UFireball::UFireball()
{
    SkillName = "Fireball";
    CooldownTime = 0.1f;
    FireballDamage = 50.0f;
    FireballSpeed = 1500.f;
    static ConstructorHelpers::FClassFinder<AProjectile_FireBall> FireballBPFinder(TEXT("/Game/WorkFolder/Bluprints/Projectiles/MyProjectile_FireBall"));
    if (FireballBPFinder.Succeeded())
    {
        ProjectileBPClass = FireballBPFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Fireball BP class!"));
    }
}

void UFireball::ActiveSkill_Implementation()
{
    if (!Owner || IsOnCooldown())
    {
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        return;
    }

    // 플레이어 컨트롤러를 통해 카메라 뷰포인트를 가져옵니다.
    FVector CameraLocation;
    FRotator CameraRotation;
    if (Owner->GetController())
    {
        Owner->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
    }
    else
    {
        CameraLocation = Owner->GetActorLocation();
        CameraRotation = Owner->GetActorRotation();
    }

    // 스폰 위치: 캐릭터의 전면 (예: 캐릭터 위치에서 전방으로 100cm)
    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 70.f + Owner->GetActorRightVector() * 30.f;
    // 진행 방향: 카메라 뷰 방향 사용
    FRotator SpawnRotation = CameraRotation;
    
    UE_LOG(LogTemp, Log, TEXT("Fireball Skill: OwnerLocation = %s"), *Owner->GetActorLocation().ToString());
    UE_LOG(LogTemp, Log, TEXT("Fireball Skill: CameraRotation = %s"), *CameraRotation.ToString());
    UE_LOG(LogTemp, Log, TEXT("Fireball Skill: SpawnLocation = %s"), *SpawnLocation.ToString());
    UE_LOG(LogTemp, Log, TEXT("Fireball Skill: SpawnRotation = %s"), *SpawnRotation.ToString());

    // ProjectileBPClass가 유효한 경우, 블루프린트로 만든 투사체 액터를 스폰
    if (ProjectileBPClass)
    {
        Owner->PlayAnimMontageByType(MontageType::DefaultAttack);

        AProjectile_FireBall* FireballProj = World->SpawnActor<AProjectile_FireBall>(ProjectileBPClass, SpawnLocation, SpawnRotation);
        if (FireballProj)
        {
            FireballProj->Shooter = Owner;
            FireballProj->DamageValue = FireballDamage;
            
            if (FireballProj->ProjectileMovement)
            {
                FireballProj->ProjectileMovement->InitialSpeed = FireballSpeed;
                FireballProj->ProjectileMovement->MaxSpeed = FireballSpeed;
            }
            UE_LOG(LogTemp, Log, TEXT("Fireball Skill: Projectile spawned successfully."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Fireball Skill: Projectile spawn failed."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UFireball::ActiveSkill_Implementation() - ProjectileBPClass is not assigned!"));
    }

    StartCooldown();
}
