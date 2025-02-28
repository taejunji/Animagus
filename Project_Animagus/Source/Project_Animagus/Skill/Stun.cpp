// Fill out your copyright notice in the Description page of Project Settings.


#include "Stun.h"
#include "../Projectile/Projectile_Stun.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"

UStun::UStun()
{
    SkillName = "Stun";
    CooldownTime = 8.0f; // 예시 쿨타임
    StunDamage = 30.0f;  // 예시 데미지
    StunSpeed = 1400.f;  // 예시 이동 속도
    StunDuration = 2.5f; // 예시 스턴 지속 시간

    // ProjectileBPClass는 에디터에서 할당 (예: MyProjectile_Stun_BP)
    ProjectileBPClass = nullptr;
}

void UStun::ActiveSkill_Implementation()
{
    if (!Owner || IsOnCooldown())
    {
        UE_LOG(LogTemp, Warning, TEXT("UStunSkill: Owner is null or skill is on cooldown."));
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("UStunSkill: World is null."));
        return;
    }

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
    
    // 스폰 위치: 캐릭터의 위치에서 전방으로 100cm, 추가로 Z축 50cm 올림 (예시)
    FVector SpawnLocation = Owner->GetActorLocation() 
                            + Owner->GetActorForwardVector() * 80.f 
                            + FVector(0.f, 0.f, 30.f);

    // 진행 방향: 캐릭터의 회전을 사용 (또는 필요시 카메라 뷰 방향)
    FRotator SpawnRotation = CameraRotation;

    UE_LOG(LogTemp, Log, TEXT("UStunSkill: SpawnLocation = %s, SpawnRotation = %s"), *SpawnLocation.ToString(), *SpawnRotation.ToString());

    if (ProjectileBPClass)
    {
        AProjectile_Stun* StunProj = World->SpawnActor<AProjectile_Stun>(ProjectileBPClass, SpawnLocation, SpawnRotation);
        if (StunProj)
        {
            StunProj->Shooter = Owner;
            StunProj->DamageValue = StunDamage;
            if (StunProj->ProjectileMovement)
            {
                StunProj->ProjectileMovement->InitialSpeed = StunSpeed;
                StunProj->ProjectileMovement->MaxSpeed = StunSpeed;
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

    StartCooldown();
}