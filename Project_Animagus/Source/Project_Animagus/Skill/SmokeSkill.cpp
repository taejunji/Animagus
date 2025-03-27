// Fill out your copyright notice in the Description page of Project Settings.


#include "SmokeSkill.h"
#include "../Projectile/Projectile_Smoke.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Project_Animagus/Character/BaseCharacter.h"

USmokeSkill::USmokeSkill()
{
    SkillName = "SmokeSkill";
    CooldownTime = 5.0f;
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

    // 플레이어(Owner)의 카메라 뷰포인트를 사용하여 스폰 위치 결정
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

    // 스폰 위치: 캐릭터의 전면 (예: 캐릭터 위치에서 전방으로 70cm)
    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 80.f + Owner->GetActorRightVector() * 30.f;

    // 진행 방향: 카메라 뷰 방향 사용
    FRotator SpawnRotation = CameraRotation;

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
