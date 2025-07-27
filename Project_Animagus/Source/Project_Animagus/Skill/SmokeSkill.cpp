// Fill out your copyright notice in the Description page of Project Settings.


#include "SmokeSkill.h"
#include "../Projectile/Projectile_Smoke.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Project_Animagus/Character/BaseCharacter.h"
#include "../AI/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"


USmokeSkill::USmokeSkill()
{
    SkillName = "SmokeSkill";
    CooldownTime = 20.0f;
    BaseCooldownTime = CooldownTime;
    SmokeProjectileSpeed = 1000.f; // 기본값, 필요에 따라 조정 가능

    //SmokeProjectileBPClass = nullptr;  // 에디터에서 할당할 것
    static ConstructorHelpers::FClassFinder<AProjectile_Smoke> SmokeBPFinder(TEXT("/Game/WorkFolder/Bluprints/Projectiles/MyProjectile_Smoke"));
    if (SmokeBPFinder.Succeeded())
    {
        SmokeProjectileBPClass = SmokeBPFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Smoke BP class!"));
    }


    SkillType = Protocol::SkillType::SMOKE;

    SkillDescription = TEXT("적의 시야를 가리거나 몸을 숨길 수 있는 연막을 소환합니다.");
}

void USmokeSkill::ActiveSkill_Implementation()
{
    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
        return;
    }



    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        return;
    }

    // 공격 애니메이션
    Owner->PlayAnimMontageByType(MontageType::DefaultAttack);

    // 스폰 위치: 캐릭터의 전면 (예: 캐릭터 위치에서 전방으로 70cm)
    FVector SpawnLocation = OwnerLocation + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 30.f;
    FRotator SpawnRotation;

    // 플레이어(Owner)의 카메라 뷰포인트를 사용하여 스폰 위치 결정
    FVector CameraLocation = OwnerLocation;
    FRotator CameraRotation = Rotation;

    // 진행 방향: 카메라 뷰 방향 사용
    SpawnRotation = CameraRotation;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner->GetInstigator();
    
    // 투사체 액터 스폰: 이때 SpawnParams를 전달하면, 액터가 생성되는 순간부터 Owner와 Instigator 값이 설정됨
    AProjectile_Smoke* SmokeProj = World->SpawnActor<AProjectile_Smoke>(
        SmokeProjectileBPClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
    
    if (SmokeProjectileBPClass)
    {
        if (SmokeProj)
        {
            SmokeProj->Shooter = Owner;
            if (SmokeProj->ProjectileMovement)
            {
                SmokeProj->ProjectileMovement->InitialSpeed = SmokeProjectileSpeed;
                SmokeProj->ProjectileMovement->MaxSpeed = SmokeProjectileSpeed;
            }
        }
    }

    // 첫 사용이면 플래그 변경
    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    StartCooldown();
}

void USmokeSkill::UpgradeSkill(int32 NewPowerUpLevel)
{
   
    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);
    
    CooldownTime = BaseCooldownTime * CooldownMultiplier;

}