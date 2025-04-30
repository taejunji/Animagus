// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicMissile.h"
#include "../Projectile/Projectile_MagicMissile.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../AI/MyAIController.h"

UMagicMissile::UMagicMissile()
{
    SkillName = "MagicMissile";
    SkillDescription = "유도 기능이 있고 데미지가 높지만 쿨타임이 긴 스킬";
    CooldownTime = 10.0f; // 
    MissileDamage = 10.0f;
    MissileSpeed = 4000.f;
    startMissileSpeed = 500.f;
    BaseCooldownTime = CooldownTime;

    static ConstructorHelpers::FClassFinder<AProjectile_MagicMissile> MaigicMissileBPFinder(TEXT("/Game/WorkFolder/Bluprints/Projectiles/MyProjectile_MagicMissile"));
    if (MaigicMissileBPFinder.Succeeded())
    {
        ProjectileBPClass = MaigicMissileBPFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load MagicMissile BP class!"));
    }

    SkillType = Protocol::SkillType::MAGICMISSILE;

}

void UMagicMissile::ActiveSkill_Implementation()
{
    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
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

    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 30.f;
    FRotator SpawnRotation;

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
        //CameraRotation = Owner->GetActorRotation();
        CameraRotation = Rotation;
    }

    // 진행 방향: 카메라 뷰 방향 사용
    SpawnRotation = CameraRotation + FRotator(2.f, 0.f, 0.f);

    UE_LOG(LogTemp, Log, TEXT("MagicMissile: OwnerLocation = %s"), *Owner->GetActorLocation().ToString());
    // UE_LOG(LogTemp, Log, TEXT("MagicMissile: CameraRotation = %s"), *CameraRotation.ToString());
    UE_LOG(LogTemp, Log, TEXT("MagicMissile: SpawnLocation = %s"), *SpawnLocation.ToString());
    UE_LOG(LogTemp, Log, TEXT("MagicMissile: SpawnRotation = %s"), *SpawnRotation.ToString());

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner->GetInstigator();
    
    // 투사체 액터 스폰: 이때 SpawnParams를 전달하면, 액터가 생성되는 순간부터 Owner와 Instigator 값이 설정됨
    AProjectile_MagicMissile* MissileProj = World->SpawnActor<AProjectile_MagicMissile>(
        ProjectileBPClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
    
    if (ProjectileBPClass)
    {
       
        if (MissileProj)
        {
            MissileProj->Shooter = Owner;
            MissileProj->DamageValue = MissileDamage;
            if (MissileProj->ProjectileMovement)
            {
                MissileProj->ProjectileMovement->InitialSpeed = startMissileSpeed;
                MissileProj->ProjectileMovement->MaxSpeed = MissileSpeed;

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

    // 첫 사용이면 플래그 변경
    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    StartCooldown();
}

void UMagicMissile::UpgradeSkill(int32 NewPowerUpLevel)
{
    // 예시: 매 단계마다 데미지는 10%씩 증가, 쿨타임은 5%씩 단축 (최소 50%까지 단축)
    // float DamageMultiplier = 1.0f + (0.10f * NewPowerUpLevel);
    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);
    
    CooldownTime = BaseCooldownTime * CooldownMultiplier;

}