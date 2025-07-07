// Fill out your copyright notice in the Description page of Project Settings.


#include "DoubleJumpItem.h"

#include "NiagaraFunctionLibrary.h"
#include "Project_Animagus/Character/BaseCharacter.h"

ADoubleJumpItem::ADoubleJumpItem()
{
    PickupEffect = nullptr;
}

void ADoubleJumpItem::OnPickedUp(class ABaseCharacter* Picker)
{
    if (!bIsPickedUp && Picker)
    {
        bIsPickedUp = true;
        
        // 플레이어의 더블 점프
        if (Picker->JumpMaxCount < 3)
        {
            Picker->JumpMaxCount++;
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
