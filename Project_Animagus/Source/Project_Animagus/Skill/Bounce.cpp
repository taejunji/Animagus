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
    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("UBounce: World is null."));
        return;
    }

    // 공격 애니메이션
    Owner->PlayAnimMontageByType(MontageType::DefaultAttack);

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
    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 30.f;


    // 진행 방향: 캐릭터의 회전(또는 필요시 카메라 뷰 방향)을 사용할 수 있습니다.
    // 여기서는 캐릭터의 회전을 사용합니다.
    FRotator SpawnRotation = CameraRotation;

    UE_LOG(LogTemp, Log, TEXT("UBounce: OwnerLocation = %s"), *Owner->GetActorLocation().ToString());
    UE_LOG(LogTemp, Log, TEXT("UBounce: SpawnLocation = %s, SpawnRotation = %s"), *SpawnLocation.ToString(), *SpawnRotation.ToString());
    
    // FActorSpawnParameters 설정: Owner와 Instigator 미리 지정
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner->GetInstigator();
    
    // 투사체 액터 스폰: 이때 SpawnParams를 전달하면, 액터가 생성되는 순간부터 Owner와 Instigator 값이 설정됨
    AProjectile_Bounce* BounceProj = World->SpawnActor<AProjectile_Bounce>(
        ProjectileBPClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
    
    if (ProjectileBPClass)
    {
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

    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    StartCooldown();
}
