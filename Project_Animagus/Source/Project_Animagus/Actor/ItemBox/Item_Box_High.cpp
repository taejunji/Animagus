// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Box_High.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "../../Item/BaseItem.h"
#include "UObject/ConstructorHelpers.h"

AItem_Box_High::AItem_Box_High()
{

    
    // 새로운 아이템 스폰 변수를 기본값으로 설정 (예: 1이면 새로운 아이템을 스폰)
    ItemSpawnVariant = 1;
}

void AItem_Box_High::BreakBox()
{
    UE_LOG(LogTemp, Log, TEXT("Item_Box_Different: Box is broken."));
    
    // 2. 정적 메쉬 숨김 처리
    if (MeshComp)
    {
        MeshComp->SetVisibility(false);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // 3. Geometry Collection 컴포넌트 활성화 (부서진 상태로 전환)
    if (FracturedComp)
    {
        FracturedComp->SetVisibility(true);
        FracturedComp->SetSimulatePhysics(true);
        FracturedComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        // 파편들이 부드럽게 분리되도록 임펄스 적용 (튜닝 필요)
        float ImpulseRadius = 100.f;
        float ImpulseStrength = 400.f;  // 적절히 튜닝
        FracturedComp->AddRadialImpulse(GetActorLocation(), ImpulseRadius, ImpulseStrength, ERadialImpulseFalloff::RIF_Linear, true);
    }

    // 4. 아이템 스폰 처리: ItemSpawnVariant 값에 따라 다른 아이템을 스폰합니다.
    TSubclassOf<ABaseItem> ItemToSpawn = nullptr;
    if (ItemSpawnVariant == 0)
    {
        ItemToSpawn = BaseItemClass;
    }
    else if (ItemSpawnVariant == 1)
    {
        ItemToSpawn = BaseItemPlusClass;
    }
    
    if (ItemToSpawn)
    {
        FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;
        FActorSpawnParameters SpawnParams;
        ABaseItem* SpawnedItem = GetWorld()->SpawnActor<ABaseItem>(ItemToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
        if (SpawnedItem)
        {
            UE_LOG(LogTemp, Log, TEXT("Item_Box_Different: Spawned item at %s"), *SpawnLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Item_Box_Different: Failed to spawn item."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Item_Box_Different: No item class assigned for spawn."));
    }

    // 5. 타이머로 일정 시간 후 액터 제거 (예: 1초 후)
    FTimerHandle TempHandle;
    GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AItem_Box_High::HandleDestroyTimer, 20.0f, false);
}
