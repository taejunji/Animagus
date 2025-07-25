#include "Fireball.h"
#include "../Projectile/Projectile_FireBall.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "../Character/PlayerCharacter.h"

#include "../AI/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"


UFireball::UFireball()
{
    SkillName = "Fireball";
    
    CooldownTime = 3.f;
    FireballDamage = 10.0f;
    FireballSpeed = 2000.f;
    static ConstructorHelpers::FClassFinder<AProjectile_FireBall> FireballBPFinder(TEXT("/Game/WorkFolder/Bluprints/Projectiles/MyProjectile_FireBall"));
    if (FireballBPFinder.Succeeded())
    {
        ProjectileBPClass = FireballBPFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Fireball BP class!"));
    }

    SkillType = Protocol::SkillType::FIREBALL;

    // 기본값 저장 (강화 전 기본 수치)
    SkillDescription = TEXT("가장 기본적인 스킬");
    BaseFireballDamage = FireballDamage;
    BaseCooldownTime = CooldownTime;
}

void UFireball::ActiveSkill_Implementation()
{
    // UE_LOG(LogTemp, Log, TEXT("UFireball::ActiveSkill_Implementation() called on %s"), *GetName());
    
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

    // 투사체 스폰 위치
    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 30.f; 
    FRotator SpawnRotation;

    FVector CameraLocation;
    FRotator CameraRotation;
    if (Owner->GetPawnType() == PawnType::PLAYER)
    {
        Owner->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
    }
    else
    {
        CameraLocation = Owner->GetActorLocation();
        CameraRotation = Rotation;
    }

    SpawnRotation = CameraRotation + FRotator(1.f, 0.f, 0.f);

    UE_LOG(LogTemp, Log, TEXT("Fireball Skill: OwnerLocation = %s"), *Owner->GetActorLocation().ToString());
    // UE_LOG(LogTemp, Log, TEXT("Fireball Skill: CameraRotation = %s"), *CameraRotation.ToString());
    UE_LOG(LogTemp, Log, TEXT("Fireball Skill: SpawnLocation = %s"), *SpawnLocation.ToString());
    UE_LOG(LogTemp, Log, TEXT("Fireball Skill: SpawnRotation = %s"), *SpawnRotation.ToString());

    // FActorSpawnParameters 설정: Owner와 Instigator 미리 지정
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner->GetInstigator();
    
    // 투사체 액터 스폰: 이때 SpawnParams를 전달하면, 액터가 생성되는 순간부터 Owner와 Instigator 값이 설정됨
    AProjectile_FireBall* FireballProj = World->SpawnActor<AProjectile_FireBall>(
        ProjectileBPClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

   //FireballProj->CollisionSphere->IgnoreActorWhenMoving(Owner, true);
    
    // ProjectileBPClass가 유효한 경우, 블루프린트로 만든 투사체 액터를 스폰
    if (ProjectileBPClass)
    {
        Owner->PlayAnimMontageByType(MontageType::DefaultAttack);

        if (FireballProj)
        {
            FireballProj->Shooter = Owner;    
            FireballProj->DamageValue = FireballDamage;
            
            if (FireballProj->ProjectileMovement)
            {
                FireballProj->ProjectileMovement->InitialSpeed = FireballSpeed;
                FireballProj->ProjectileMovement->MaxSpeed = FireballSpeed;

                FVector Dir = FireballProj->ProjectileMovement->Velocity.GetSafeNormal();
                FireballProj->ProjectileMovement->Velocity = Dir * FireballSpeed;
            }
            UE_LOG(LogTemp, Log, TEXT("Fireball Skill: Projectile spawned successfully."));
            UE_LOG(LogTemp, Log, TEXT("FireballProj->Shooter set to: %s"), 
          FireballProj->Shooter ? *FireballProj->Shooter->GetName() : TEXT("NULL"));
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

    // 첫 사용이면 플래그 변경
    if (bFirstUse)
    {
        bFirstUse = false;
    } 
    StartCooldown();
}

void UFireball::UpgradeSkill(int32 NewPowerUpLevel)
{
    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);

    FireballDamage = BaseFireballDamage + (3.f * NewPowerUpLevel);
    CooldownTime = BaseCooldownTime * CooldownMultiplier;
    FireballSpeed = 2000.f + (150.f * NewPowerUpLevel);

    UE_LOG(LogTemp, Log, TEXT("Fireball upgraded: PowerUpLevel %d, Damage: %f, Cooldown: %f"), 
        NewPowerUpLevel, FireballDamage, CooldownTime);
}



//// 투사체 스폰 위치
//FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 30.f;
//FRotator SpawnRotation;
//
//// 플레이어와 AI 분기 처리
//if (Owner->IsPlayerControlled())
//{
//    FVector CameraLocation;
//    FRotator CameraRotation;
//    if (Owner->GetController())
//    {
//        Owner->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
//    }
//    else
//    {
//        CameraLocation = Owner->GetActorLocation();
//        CameraRotation = Owner->GetActorRotation();
//    }
//    SpawnRotation = CameraRotation + FRotator(2.f, 0.f, 0.f);
//}
//else
//{
//    AMyAIController* AIController = Cast<AMyAIController>(Owner->GetController());
//    ABaseCharacter* TargetCharacter = nullptr;
//
//    if (AIController)
//    {
//        UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
//        if (BBComp && AIController->TargetKey.SelectedKeyName.IsValid())
//        {
//            TargetCharacter = Cast<ABaseCharacter>(BBComp->GetValueAsObject(AIController->TargetKey.SelectedKeyName));
//        }
//    }
//
//    if (TargetCharacter)
//    {
//        FVector DirectionToTarget = (TargetCharacter->GetActorLocation() - SpawnLocation).GetSafeNormal();
//        SpawnRotation = DirectionToTarget.Rotation();
//    }
//    else
//    {
//        SpawnRotation = Owner->GetActorRotation();
//    }
//}
