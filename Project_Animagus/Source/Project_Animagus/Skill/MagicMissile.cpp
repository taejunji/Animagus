// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicMissile.h"
#include "../Projectile/Projectile_MagicMissile.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"


UMagicMissile::UMagicMissile()
{
    SkillName = "MagicMissile";
    SkillDescription = "유도 기능이 있고 데미지가 높지만 쿨타임이 긴 스킬";
    CooldownTime = 5.0f; // 
    MissileDamage = 10.0f;
    MissileSpeed = 1800.f;
    HomingActivationRadius = 3000.f;          // 30m
    HomingAccelerationMagnitude = 5000.f;
    static ConstructorHelpers::FClassFinder<AProjectile_MagicMissile> MaigicMissileBPFinder(TEXT("/Game/WorkFolder/Bluprints/Projectiles/MyProjectile_MagicMissile"));
    if (MaigicMissileBPFinder.Succeeded())
    {
        ProjectileBPClass = MaigicMissileBPFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load MagicMissile BP class!"));
    }
}

void UMagicMissile::ActiveSkill_Implementation()
{
    if (!Owner || IsOnCooldown())
    {
        UE_LOG(LogTemp, Warning, TEXT("MagicMissile: Owner is null or skill is on cooldown."));
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("MagicMissile: World is null."));
        return;
    }
    
    // 공격 애니메이션
    Owner->PlayAnimMontageByType(MontageType::DefaultAttack);

    // TPS 기준: 스폰 위치는 캐릭터 전면(약간 위쪽)에서 생성하고,
    // 진행 방향은 플레이어 컨트롤러의 카메라 뷰포인트 방향을 사용합니다.
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

    // 스폰 위치: 캐릭터 위치에서 전방 100cm, 동시에 Z축으로 50cm 올림
    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f + Owner->GetActorRightVector() * 30.f;

    // 진행 방향: 카메라 뷰 방향 사용
    FRotator SpawnRotation = CameraRotation;

    UE_LOG(LogTemp, Log, TEXT("MagicMissile: OwnerLocation = %s"), *Owner->GetActorLocation().ToString());
    UE_LOG(LogTemp, Log, TEXT("MagicMissile: CameraRotation = %s"), *CameraRotation.ToString());
    UE_LOG(LogTemp, Log, TEXT("MagicMissile: SpawnLocation = %s"), *SpawnLocation.ToString());
    UE_LOG(LogTemp, Log, TEXT("MagicMissile: SpawnRotation = %s"), *SpawnRotation.ToString());

    if (ProjectileBPClass)
    {
        AProjectile_MagicMissile* MissileProj = World->SpawnActor<AProjectile_MagicMissile>(ProjectileBPClass, SpawnLocation, SpawnRotation);
        if (MissileProj)
        {
            MissileProj->Shooter = Owner;
            MissileProj->DamageValue = MissileDamage;
            if (MissileProj->ProjectileMovement)
            {
                MissileProj->ProjectileMovement->InitialSpeed = MissileSpeed;
                MissileProj->ProjectileMovement->MaxSpeed = MissileSpeed;
                // 초기에는 유도 기능 off, MissileProj의 Tick()에서 유도 대상이 있으면 활성화하도록 함.
                MissileProj->ProjectileMovement->bIsHomingProjectile = false;
                MissileProj->ProjectileMovement->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
            }
            // 추가로, MissileProj에 HomingActivationRadius 값을 전달할 수도 있음(만약 필요하면)
            // 예: MissileProj->HomingActivationRadius = HomingActivationRadius;
            
            UE_LOG(LogTemp, Log, TEXT("MagicMissile: Projectile spawned successfully."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MagicMissile: Projectile spawn failed."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UMagicMissile::ActiveSkill_Implementation() - ProjectileBPClass is not assigned!"));
    }

    StartCooldown();
}
