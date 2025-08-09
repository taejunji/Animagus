// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckShrinkingZone.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../GameMode/BattleGameMode.h"
#include "../Actor/Zones/ShrinkingZone.h"
#include "Kismet/GameplayStatics.h"

UBTService_CheckShrinkingZone::UBTService_CheckShrinkingZone()
{
    NodeName = TEXT("Check ShrinkingZone");
    bNotifyTick = true;
    Interval = 2.f; 
    RandomDeviation = 0.5; // 약간 랜덤 지연 추가 가능
}

void UBTService_CheckShrinkingZone::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (nullptr == BlackboardComp) return;

    ABattleGameMode* BattleMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!BattleMode || !BattleMode->ShrinkingZone) return;

    FVector PatrolLoc = BlackboardComp->GetValueAsVector(patrol_pos_key.SelectedKeyName);

    float Distance = FVector::Dist(FVector::ZeroVector, PatrolLoc);

    // ShrinkingZone의 반지름보다 거리가 크면 실패하도록
    if (Distance >= BattleMode->ShrinkingZone->CurrentRadius)
    {   
        BlackboardComp->ClearValue(patrol_pos_key.SelectedKeyName);
    }
}
