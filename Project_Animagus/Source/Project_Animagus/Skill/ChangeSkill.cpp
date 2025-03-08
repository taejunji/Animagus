// Fill out your copyright notice in the Description page of Project Settings.


#include "ChangeSkill.h"
#include "../Projectile/Projectile_change.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"

UChangeSkill::UChangeSkill()
{
    SkillName = "ChangeSkill";  // 스킬 이름 설정
    CooldownTime = 5.0f;        // 쿨타임 5초
    ChangeDamage = 0.0f;        // 데미지 없음(교환 효과 목적)
    ChangeSpeed = 1000.f;       // 투사체 속도

    ProjectileBPClass = nullptr;  // 에디터에서 할당할 것
}

void UChangeSkill::ActiveSkill_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("UChangeSkill::ActiveSkill_Implementation() 호출됨."));

    if (!Owner || IsOnCooldown())
    {
        UE_LOG(LogTemp, Warning, TEXT("UChangeSkill: Owner가 null이거나 스킬이 쿨타임 중임."));
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("UChangeSkill: World가 null임."));
        return;
    }

    // 스폰 위치: 캐릭터 위치에서 전방 80cm, 위로 20cm 오프셋
    FVector SpawnLocation = Owner->GetActorLocation() 
                            + Owner->GetActorForwardVector() * 80.f 
                            + FVector(0.f, 0.f, 20.f);
                            
    // 진행 방향: 카메라의 뷰포인트를 사용
    FVector CameraLocation;
    FRotator CameraRotation;
    if (Owner->GetController())
    {
        Owner->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
        UE_LOG(LogTemp, Log, TEXT("UChangeSkill: CameraLocation = %s, CameraRotation = %s"), *CameraLocation.ToString(), *CameraRotation.ToString());
    }
    else
    {
        CameraRotation = Owner->GetActorRotation();
        UE_LOG(LogTemp, Log, TEXT("UChangeSkill: Controller 없음, CameraRotation = %s"), *CameraRotation.ToString());
    }
    
    FRotator SpawnRotation = CameraRotation;  // 진행 방향은 카메라의 회전값 사용

    UE_LOG(LogTemp, Log, TEXT("UChangeSkill: SpawnLocation = %s, SpawnRotation = %s"), *SpawnLocation.ToString(), *SpawnRotation.ToString());

    if (ProjectileBPClass)
    {
        AProjectile_change* Projectile = World->SpawnActor<AProjectile_change>(ProjectileBPClass, SpawnLocation, SpawnRotation);
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
    
    StartCooldown();
}


