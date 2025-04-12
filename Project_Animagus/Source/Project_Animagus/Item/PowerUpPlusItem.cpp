// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpPlusItem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Character/BaseCharacter.h"

APowerUpPlusItem::APowerUpPlusItem()
{
    // 에디터에서 할당하도록 기본값은 nullptr로 설정
    PickupEffect = nullptr;
}

void APowerUpPlusItem::OnPickedUp(ABaseCharacter* Picker)
{
    if (!bIsPickedUp && Picker)
    {
        bIsPickedUp = true;
        UE_LOG(LogTemp, Log, TEXT("APowerUpItem: Picked up by %s"), *Picker->GetName());

        // 플레이어의 파워업 상태 증가 처리
        Picker->IncreasePowerUpLevel();
        Picker->IncreasePowerUpLevel();
        
        // 피크업 이펙트 재생
        if (PickupEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupEffect, GetActorLocation(), GetActorRotation());
        }

        // 아이템 소멸
        DestroyItem();
    }
}