#include "PowerUpItem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Character/BaseCharacter.h"

APowerUpItem::APowerUpItem()
{
    // 에디터에서 할당하도록 기본값은 nullptr로 설정
    PickupEffect = nullptr;
}

void APowerUpItem::OnPickedUp(ABaseCharacter* Picker)
{
    if (!bIsPickedUp && Picker)
    {
        bIsPickedUp = true;
        UE_LOG(LogTemp, Log, TEXT("APowerUpItem: Picked up by %s"), *Picker->GetName());

        // 플레이어의 파워업 상태 증가 처리
        Picker->IncreasePowerUpLevel();

        // 피크업 이펙트 재생
        if (PickupEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupEffect, GetActorLocation()+ FVector(0.f, 0.f, 20.f), GetActorRotation());
        }
        

        
        // 아이템 소멸
        DestroyItem();
    }
}
