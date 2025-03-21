// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldSkill.h"
#include "Project_Animagus/Actor/Shield/ShieldActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Project_Animagus/Character/BaseCharacter.h"

UShieldSkill::UShieldSkill()
{
    SkillName = "ShieldSkill";
    // 쿨타임과 지속 시간은 디폴트 값으로 설정, 필요에 따라 에디터에서 조정 가능
    CooldownTime = 10.0f;
    ShieldDuration = 5.0f;

    ShieldActorBPClass = nullptr; // 에디터에서 보호막 액터 블루프린트 클래스를 할당할 것

 
}

void UShieldSkill::ActiveSkill_Implementation()
{

    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World || !ShieldActorBPClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: World null or ShieldActorBPClass not assigned."));
        return;
    }

    
    
    // 스폰 위치: 스킬 시전자의 위치
    FVector SpawnLocation = Owner->GetActorLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;

    // 보호막 액터 스폰
    if (AShieldActor* ShieldActor = World->SpawnActor<AShieldActor>(ShieldActorBPClass, SpawnLocation, SpawnRotation))
    {
        // 스킬 시전자를 보호막 액터의 ShieldOwner로 설정하여, Tick()에서 따라다니게 함
        ShieldActor->SetShieldOwner(Owner);
        // 필요 시, 보호막 지속 시간을 업데이트 (보호막 액터의 ShieldDuration 변수)
        ShieldActor->ShieldDuration = ShieldDuration;
    }

    // 첫 사용이면 플래그 변경
    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    // 스킬 사용 후 쿨타임 시작
    StartCooldown();
}
