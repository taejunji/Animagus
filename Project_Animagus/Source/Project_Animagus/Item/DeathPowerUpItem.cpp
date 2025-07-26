// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathPowerUpItem.h"
#include "NiagaraFunctionLibrary.h"
#include "../Character/BaseCharacter.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"

ADeathPowerUpItem::ADeathPowerUpItem()
{
    // 생성 직후 충돌 비활성화
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (AActor* Spawner = GetOwner())
    {
        CollisionComp->IgnoreActorWhenMoving(Spawner, true);
    }

    SetItemType(Protocol::ItemType::POWERUP);
}

void ADeathPowerUpItem::BeginPlay()
{
    Super::BeginPlay();

    if (AActor* Spawner = GetOwner())
    {
        CollisionComp->IgnoreActorWhenMoving(Spawner, true);
    }
    
    // 1.5초 뒤에 충돌 활성화 & 이펙트 재생
    GetWorldTimerManager().SetTimer(
        EnableTimerHandle,
        this,
        &ADeathPowerUpItem::EnableCollisionAndEffect,
        1.5f,
        false
    );

    SetItemType(Protocol::ItemType::POWERUP);
}

void ADeathPowerUpItem::EnableCollisionAndEffect()
{
    // 1) 충돌 활성화
    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // 2) 나이아가라 이펙트 재생 (아이템 위치에)
    if (DelayActivateEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this,
            DelayActivateEffect,
            GetActorLocation(),
            FRotator::ZeroRotator
        );
    }
}

void ADeathPowerUpItem::OnItemOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    if (OtherActor == GetOwner())
    {
        return;
    }
    
    // 활성화 전엔 무시
    if (GetWorldTimerManager().IsTimerActive(EnableTimerHandle))
    {
        return;
    }

    if (OtherActor == GetOwner())
    {
        return;
    }

    ABaseCharacter* Picker = Cast<ABaseCharacter>(OtherActor);
    if (Picker)
    {
        if (Picker->GetPawnType() != PawnType::NETWORK)
        {
            for (int32 i = 0; i < StoredPowerUpCount; ++i)
            {
                Picker->IncreasePowerUpLevel();
            }
        }

        // 기본 획득 이펙트·사운드
        Super::OnItemOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    }
}

