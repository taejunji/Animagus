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
    CooldownTime = 5.0f;        // 쿨타임 5초
    ChangeDamage = 0.0f;        // 데미지 없음(교환 효과 목적)
    ChangeSpeed = 1000.f;       // 투사체 속도

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
    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 20.f;
    FRotator SpawnRotation;

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
        CameraRotation = Rotation;
    }

    // 진행 방향: 카메라 뷰 방향 사용
    SpawnRotation = CameraRotation;

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
    // 예시: 매 단계마다 데미지는 10%씩 증가, 쿨타임은 5%씩 단축 (최소 50%까지 단축)
    // float DamageMultiplier = 1.0f + (0.10f * NewPowerUpLevel);
    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);
    
    CooldownTime = BaseCooldownTime * CooldownMultiplier;

}
