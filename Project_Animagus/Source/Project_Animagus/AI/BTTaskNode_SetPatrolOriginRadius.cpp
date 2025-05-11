// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SetPatrolOriginRadius.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/BaseCharacter.h"
#include "../GameMode/BattleGameMode.h"
#include "../AI/MyAIController.h"
#include "Kismet/GameplayStatics.h"


UBTTaskNode_SetPatrolOriginRadius::UBTTaskNode_SetPatrolOriginRadius()
{
    NodeName = TEXT("Set Random Orign Radius");
}

EBTNodeResult::Type UBTTaskNode_SetPatrolOriginRadius::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) {
        return EBTNodeResult::Failed;
    }

    ABattleGameMode* BattleMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!BattleMode) {
        return EBTNodeResult::Failed;
    }

    ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (Character == nullptr) {
        return EBTNodeResult::Failed;
    }

    // 1. 여러 개의 스폰 지점과 반경을 배열로 선언
    TArray<FVector> SpawnOrigins = { 
         FVector(0.0f,0.0f, 1443.468667),   // [0] : 전투존
                 
         FVector(-4370.0f,-4545.0f, 810.f), // [1] 
         FVector(0.0f,-4545.0f, 810.f),     // [2]
         FVector(4370.0f,-4545.0f, 810.f),  // [3]
         FVector(-4370.0f, 0.0f, 810.f),    // [4]
         FVector(4370.0f, 0.0f, 810.f),     // [5]
         FVector(-4370.0f,4545.0f, 810.f),  // [6]
         FVector(0.0f,4545.0f, 810.f),      // [7]
         FVector(4370.0f,4545.0f, 810.f),   // [8]         
    };

    TArray<float> SpawnRadii = {
        1300.f,
                
        2100.f,
        2100.f,
        2100.f,
        2100.f,
        2100.f,
        2100.f,
        2100.f,
        2100.f,        
    };
    
    FVector MyLocation = Character->GetActorLocation();

    int32 SelectedIndex = 0;

    // 1분 30초 넘어가면 중앙으로 가도록
    if (BattleMode->GetCurrentRoundTime() < 90.f)
    {
        TArray<FIndexDistancePair> DistancePairs;

        // [0] ~ [8] 사이 거리 측정
        for (int32 i = 0; i < SpawnOrigins.Num(); ++i)
        {
            // 거리의 제곱값만 계산 (제곱근 계산 안 함) -> 비교 목적일 경우 더 빠름
            float DistSq = FVector::DistSquared(MyLocation, SpawnOrigins[i]);

            FIndexDistancePair Pair;
            Pair.Index = i;
            Pair.DistanceSq = DistSq;

            DistancePairs.Add(Pair);
        }

        // 거리순 정렬
        DistancePairs.Sort(FCompareDistance());

        // 가까운 순으로 최대 4개 중 하나 선택
        int32 NumCandidates = FMath::Min(4, DistancePairs.Num());
        int32 RandomChoice = FMath::RandRange(0, NumCandidates - 1);

        SelectedIndex = DistancePairs[RandomChoice].Index;
    }
    else
    {
        // 2분 이상이면 무조건 [0] 선택
        SelectedIndex = 0;
    }

    // DrawDebugSphere(GetWorld(), SpawnOrigins[SelectedIndex], 500, 16, FColor::Blue, false, 5.f);


    // 선택된 위치와 반경을 Blackboard에 저장
    BlackboardComp->SetValueAsVector(random_patrol_pos_key.SelectedKeyName, SpawnOrigins[SelectedIndex]);
    BlackboardComp->SetValueAsFloat(random_patrol_radius_key.SelectedKeyName, FMath::RandRange(600.0f, SpawnRadii[SelectedIndex]));


    return EBTNodeResult::Succeeded;
}

/*
    // 2. 랜덤하게 한 스폰 지역을 선택
    int32 RandomIndex = FMath::RandRange(0, SpawnOrigins.Num() - 1);
    FVector OriginVector = SpawnOrigins[RandomIndex];
    float SearchRadius = FMath::RandRange(600.0f, SpawnRadii[RandomIndex]);

        // 3. BB에 Origin과 Radius 저장
    BlackboardComp->SetValueAsVector(random_patrol_pos_key.SelectedKeyName, OriginVector);
    BlackboardComp->SetValueAsFloat(random_patrol_radius_key.SelectedKeyName, SearchRadius);
*/