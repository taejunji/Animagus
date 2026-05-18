// Fill out your copyright notice in the Description page of Project Settings.


#include "HealPlusItem.h"
#include "NiagaraFunctionLibrary.h"
#include "Project_Animagus/Character/BaseCharacter.h"

AHealPlusItem::AHealPlusItem()
{
    PickupEffect = nullptr;
    SetItemType(Protocol::ItemType::HEAL);
}

void AHealPlusItem::BeginPlay()
{
    Super::BeginPlay();
    
    SetItemType(Protocol::ItemType::HEAL);
}

void AHealPlusItem::OnPickedUp(class ABaseCharacter* Picker)
{
    if (!bIsPickedUp && Picker)
    {
        bIsPickedUp = true;
        UE_LOG(LogTemp, Log, TEXT("APowerUpItem: Picked up by %s"), *Picker->GetName());

        if (Picker->GetPawnType() != PawnType::NETWORK)
        {
            // 플레이어의 파워업 상태 증가 처리
            Picker->PickHealingPlusItem();
            SendItemPickedUp2Server(Picker);
        }

        // 피크업 이펙트 재생
        if (PickupEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupEffect, GetActorLocation()+ FVector(0.f, 0.f, 20.f), GetActorRotation());
        }
        
        // 아이템 소멸
        DestroyItem();
    } 
}
