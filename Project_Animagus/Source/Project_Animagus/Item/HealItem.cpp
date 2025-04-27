// Fill out your copyright notice in the Description page of Project Settings.


#include "HealItem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Character/BaseCharacter.h"

AHealItem::AHealItem()
{
    PickupEffect = nullptr;
}

void AHealItem::OnPickedUp(class ABaseCharacter* Picker)
{
    if (!bIsPickedUp && Picker)
    {
        bIsPickedUp = true;
        UE_LOG(LogTemp, Log, TEXT("APowerUpItem: Picked up by %s"), *Picker->GetName());

        // 플레이어의 파워업 상태 증가 처리
        Picker->PickHealingItem();

        // 피크업 이펙트 재생
        if (PickupEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupEffect, GetActorLocation()+ FVector(0.f, 0.f, 20.f), GetActorRotation());
        }
        

        
        // 아이템 소멸
        DestroyItem();
    }
    
}
