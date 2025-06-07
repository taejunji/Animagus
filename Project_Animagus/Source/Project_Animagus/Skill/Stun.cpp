// Fill out your copyright notice in the Description page of Project Settings.


#include "Stun.h"
#include "../Projectile/Projectile_Stun.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../AI/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UStun::UStun()
{
    SkillName = "Stun";
    CooldownTime = 8.0f; // 예시 쿨타임
    StunDamage = 10.0f;  // 예시 데미지
    StunSpeed = 2500.f;  // 예시 이동 속도
    StunDuration = 1.67f; // 예시 스턴 지속 시간

    // ProjectileBPClass는 에디터에서 할당 (예: MyProjectile_Stun_BP)
    ProjectileBPClass = nullptr;
    SkillDescription = TEXT("피격시 움직임을 제한 하는 투사체를 발사합니다.");
    BaseCooldownTime = CooldownTime;
    BaseStunDuration = StunDuration;
}

void UStun::ActiveSkill_Implementation()
{
    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("UStunSkill: World is null."));
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
            SpawnRotation = Owner->GetActorRotation() + FRotator(1.f, 0.f, 0.f);;
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
        SpawnRotation = CameraRotation + FRotator(1.f, 0.f, 0.f);;
    }


    UE_LOG(LogTemp, Log, TEXT("UStunSkill: SpawnLocation = %s, SpawnRotation = %s"), *SpawnLocation.ToString(), *SpawnRotation.ToString());

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner;
    SpawnParams.Instigator = Owner->GetInstigator();
    
    // 투사체 액터 스폰: 이때 SpawnParams를 전달하면, 액터가 생성되는 순간부터 Owner와 Instigator 값이 설정됨
    AProjectile_Stun* StunProj = World->SpawnActor<AProjectile_Stun>(
        ProjectileBPClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );
    if (ProjectileBPClass)
    {
        if (StunProj)
        {
            StunProj->Shooter = Owner;
            StunProj->DamageValue = StunDamage;
            if (StunProj->ProjectileMovement)
            {
                StunProj->ProjectileMovement->InitialSpeed = StunSpeed;
                StunProj->ProjectileMovement->MaxSpeed = StunSpeed;
                FVector Dir = StunProj->ProjectileMovement->Velocity.GetSafeNormal();
                StunProj->ProjectileMovement->Velocity = Dir * StunSpeed;
            }
            // 투사체의 OnHit()에서 대상 캐릭터에 대해 ApplyStun(StunDuration)를 호출하도록 설계합니다.
            UE_LOG(LogTemp, Log, TEXT("UStunSkill: Stun projectile spawned successfully: %s"), *StunProj->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UStunSkill: Failed to spawn stun projectile."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UStunSkill::ActiveSkill_Implementation() - ProjectileBPClass is not assigned!"));
    }
    
    // 첫 사용이면 플래그 변경
    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    StartCooldown();
}
void UStun::UpgradeSkill(int32 NewPowerUpLevel)
{
   
    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);
    
    CooldownTime = BaseCooldownTime * CooldownMultiplier;

}