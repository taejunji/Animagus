// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeSkill.h"
#include "../Projectile/Projectile_change.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../AI/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UChangeSkill::UChangeSkill()
{
    SkillName = "ChangeSkill";  // 스킬 이름 설정
    CooldownTime = 10.0f;        // 쿨타임 5초
    ChangeDamage = 1.0f;        // 데미지 없음(교환 효과 목적)
    ChangeSpeed = 1500.f;       // 투사체 속도
    SkillDescription = TEXT("적중한 적 바로 앞으로 순간이동합니다.");
    BaseCooldownTime = CooldownTime;
    //ProjectileBPClass = nullptr;  // 에디터에서 할당할 것
    static ConstructorHelpers::FClassFinder<AProjectile_change> ChangeBPFinder(TEXT("/Game/WorkFolder/Bluprints/Projectiles/MyProjectile_change"));
    if (ChangeBPFinder.Succeeded())
    {
        ProjectileBPClass = ChangeBPFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Change BP class!"));
    }

    BaseChangeSpeed = ChangeSpeed;
    //ProjectileBPClass = nullptr;  // 에디터에서 할당할 것

    SkillType = Protocol::SkillType::CHANGE;
}

void UChangeSkill::ActiveSkill_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("UChangeSkill::ActiveSkill_Implementation() 호출됨."));

    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("UChangeSkill: World가 null임."));
        return;
    }

    // 공격 애니메이션
    Owner->PlayAnimMontageByType(MontageType::DefaultAttack);

    // 투사체 스폰 위치
    FVector SpawnLocation = OwnerLocation + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 20.f;
    FRotator SpawnRotation;

    // 플레이어 컨트롤러를 통해 카메라 뷰포인트를 가져옵니다.
    FVector CameraLocation = OwnerLocation;
    FRotator CameraRotation = Rotation;

    // 진행 방향: 카메라 뷰 방향 사용
    SpawnRotation = CameraRotation + FRotator(1.f, 0.f, 0.f);;

    UE_LOG(LogTemp, Log, TEXT("UChangeSkill: SpawnLocation = %s, SpawnRotation = %s"), *SpawnLocation.ToString(), *SpawnRotation.ToString());

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner->GetInstigator();
    
    // 투사체 액터 스폰: 이때 SpawnParams를 전달하면, 액터가 생성되는 순간부터 Owner와 Instigator 값이 설정됨
    AProjectile_change* Projectile = World->SpawnActor<AProjectile_change>(
        ProjectileBPClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
    
    if (ProjectileBPClass)
    {
        if (Projectile)
        {
            Projectile->Shooter = Owner;
            Projectile->DamageValue = ChangeDamage;
            if (Projectile->ProjectileMovement)
            {
                Projectile->ProjectileMovement->InitialSpeed = ChangeSpeed;
                Projectile->ProjectileMovement->MaxSpeed = ChangeSpeed;
                FVector Dir = Projectile->ProjectileMovement->Velocity.GetSafeNormal();
                Projectile->ProjectileMovement->Velocity = Dir * ChangeSpeed;
            }
            UE_LOG(LogTemp, Log, TEXT("UChangeSkill: Spawned projectile: %s"), *Projectile->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UChangeSkill: 투사체 생성 실패."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UChangeSkill: ProjectileBPClass 할당되지 않음."));
    }

        // 첫 사용이면 플래그 변경
        if (bFirstUse)
        {
            bFirstUse = false;
        }

    
    StartCooldown();
}

void UChangeSkill::UpgradeSkill(int32 NewPowerUpLevel)
{
    int32 Level = FMath::Clamp(NewPowerUpLevel, 0, 14);

    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);
    CooldownTime = BaseCooldownTime * CooldownMultiplier;

    ChangeSpeed = BaseChangeSpeed + (Level * 400.f);
   
    UE_LOG(LogTemp, Warning, TEXT("changeSpeed %f"), ChangeSpeed);
    
}
