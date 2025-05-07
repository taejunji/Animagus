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