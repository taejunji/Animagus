// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindTarget.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/BaseCharacter.h"
#include "Engine/OverlapResult.h"
#include "../Item/BaseItem.h"

UBTService_FindTarget::UBTService_FindTarget()
{
    NodeName = TEXT("Find ItemTarget Service");
    Interval = 0.5f; // 주기
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // 주인 가져오기
    APawn* LocalPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (LocalPawn == nullptr) {
        return;
    }

    UWorld* World = LocalPawn->GetWorld();
    if (World == nullptr) {
        return;
    }

    FVector Location = LocalPawn->GetActorLocation();
    // TargetKey가 있는지 확인
    ABaseItem* TargetItem = Cast<ABaseItem>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ItemTarget_key.SelectedKeyName));

    // 색상 결정 (빨강: 타겟 있음, 파랑: 타겟 없음)
    bool bHasTarget = (TargetItem != nullptr);
    FColor DebugColor = bHasTarget ? FColor::Red : FColor::Blue; 
    // DrawDebugSphere(World, Location, search_radius, 16, DebugColor, false, 0.2f); 

    if (TargetItem) return;

    TArray<FOverlapResult> OverlapResults; 
    
    FCollisionQueryParams CollisionQueryParam(NAME_None, false, LocalPawn);

    bool bResult = World->OverlapMultiByChannel(
        OverlapResults,
        Location,
        FQuat::Identity,
        ECollisionChannel::ECC_WorldDynamic, // ECC_WorldDynamic: 동적 월드 객체들과 충돌을 검사합니다. (예: 움직이는 Actor들)
        FCollisionShape::MakeSphere(search_radius),
        CollisionQueryParam
    );

    // 이미 세팅된 아이템이 있는지 확인
    if (bResult)
    {
        for (FOverlapResult& OverlapResult : OverlapResults)
        {
            if (ABaseItem* Item = Cast<ABaseItem>(OverlapResult.GetActor()))
            {
                FVector ActorLocation = Item->GetActorLocation();

                // Z값이 특정 범위를 벗어나면 타겟 설정하지 않음
                if (FMath::Abs(ActorLocation.Z - Location.Z) <= max_z_difference) // maxZDifference는 허용하는 Z 범위
                {
                    // DrawDebugSphere(World, ActorLocation, 500, 16, FColor::Magenta, true, -1.f);
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject(ItemTarget_key.SelectedKeyName, Item);
                    return;
                }
            }
        }
    }
}


#if 0
// -> 투사체 감지 코드
#include "../Skill/Fireball.h"
#include "../Projectile/ProjectileBase.h"


bool bResult = World->OverlapMultiByChannel(
    OverlapResults,
    Location,
    FQuat::Identity,
    ECollisionChannel::ECC_GameTraceChannel1, // ECC_WorldDynamic: 동적 월드 객체들과 충돌을 검사합니다. (예: 움직이는 Actor들)
    FCollisionShape::MakeSphere(search_radius),
    CollisionQueryParam
);

if (bResult)
{
    for (FOverlapResult& OverlapResult : OverlapResults)
    {
        AProjectileBase* pro = Cast<AProjectileBase>(OverlapResult.GetActor());
        if (pro)
        {
            DrawDebugSphere(World, Location, search_radius, 16, FColor::Red, false, 0.2f);
            return;
        }
    }
}

#endif