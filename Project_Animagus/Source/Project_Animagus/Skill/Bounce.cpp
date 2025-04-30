#include "Bounce.h"
#include "../Projectile/Projectile_Bounce.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../AI/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBounce::UBounce()
{
    SkillName = "Bounce";
    CooldownTime = 5.0f; // 예시로 6초 쿨타임
    BounceDamage = 10.0f;
    BounceSpeed = 1000.f; // 필요에 따라 조정
    BaseCooldownTime = CooldownTime;

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

    // 투사체 스폰 위치
    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 30.f;
    FRotator SpawnRotation;

    // AI가 호출한 경우
    if (AMyAIController* AIController = Cast<AMyAIController>(Owner->GetController()))
    {
        ABaseCharacter* TargetCharacter = nullptr;
        UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
        if (BBComp && AIController->TargetKey.SelectedKeyName.IsValid())
        {
            TargetCharacter = Cast<ABaseCharacter>(BBComp->GetValueAsObject(AIController->TargetKey.SelectedKeyName));
        }
        if (TargetCharacter)
        {
            FVector DirectionToTarget = (TargetCharacter->GetActorLocation() - SpawnLocation).GetSafeNormal();
            SpawnRotation = DirectionToTarget.Rotation();
        }
        else
        {
            // 타겟이 없다면 AI Panw이 바라보는 방향으로 발사
            SpawnRotation = Owner->GetActorRotation();
        }
    }
    else // Player 혹은 Network가 호출한 경우
    {
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

        // 진행 방향: 카메라 뷰 방향 사용
        SpawnRotation = CameraRotation;
    }


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

void UBounce::UpgradSkill(int32 NewPowerUpLevel)
{
    // 예시: 매 단계마다 데미지는 10%씩 증가, 쿨타임은 5%씩 단축 (최소 50%까지 단축)
    // float DamageMultiplier = 1.0f + (0.10f * NewPowerUpLevel);
    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);
    
    CooldownTime = BaseCooldownTime * CooldownMultiplier;
}
