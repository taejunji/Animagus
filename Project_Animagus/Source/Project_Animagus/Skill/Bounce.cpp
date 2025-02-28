#include "Bounce.h"
#include "../Projectile/Projectile_Bounce.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"

UBounce::UBounce()
{
    SkillName = "Bounce";
    CooldownTime = 5.0f; // 예시로 6초 쿨타임
    BounceDamage = 10.0f;
    BounceSpeed = 1000.f; // 필요에 따라 조정
}

void UBounce::ActiveSkill_Implementation()
{
    if (!Owner || IsOnCooldown())
    {
        UE_LOG(LogTemp, Warning, TEXT("UBounce: Owner is null or skill is on cooldown."));
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("UBounce: World is null."));
        return;
    }

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
    
    // 스폰 위치: 캐릭터의 전면 오프셋 + 약간 위쪽 (예: 전방 80cm, Z축 20cm)
    FVector SpawnLocation = Owner->GetActorLocation() 
        + Owner->GetActorForwardVector() * 80.f 
        + FVector(0.f, 0.f, 20.f);

    // 진행 방향: 캐릭터의 회전(또는 필요시 카메라 뷰 방향)을 사용할 수 있습니다.
    // 여기서는 캐릭터의 회전을 사용합니다.
    FRotator SpawnRotation = CameraRotation;

    UE_LOG(LogTemp, Log, TEXT("UBounce: OwnerLocation = %s"), *Owner->GetActorLocation().ToString());
    UE_LOG(LogTemp, Log, TEXT("UBounce: SpawnLocation = %s, SpawnRotation = %s"), *SpawnLocation.ToString(), *SpawnRotation.ToString());

    if (ProjectileBPClass)
    {
        AProjectile_Bounce* BounceProj = World->SpawnActor<AProjectile_Bounce>(ProjectileBPClass, SpawnLocation, SpawnRotation);
        if (BounceProj)
        {
            BounceProj->Shooter = Owner;
            BounceProj->DamageValue = BounceDamage;
            if (BounceProj->ProjectileMovement)
            {
                BounceProj->ProjectileMovement->InitialSpeed = BounceSpeed;
                BounceProj->ProjectileMovement->MaxSpeed = BounceSpeed;
            }
            UE_LOG(LogTemp, Log, TEXT("UBounce: Projectile spawned successfully: %s"), *BounceProj->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UBounce: Projectile spawn failed."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UBounce::ActiveSkill_Implementation() - ProjectileBPClass is not assigned!"));
    }

    StartCooldown();
}
