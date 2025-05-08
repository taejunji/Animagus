// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SetPatrolOriginRadius.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTaskNode_SetPatrolOriginRadius::UBTTaskNode_SetPatrolOriginRadius()
{
    NodeName = TEXT("Set Random Orign Radius");
}

EBTNodeResult::Type UBTTaskNode_SetPatrolOriginRadius::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    // 1. 여러 개의 스폰 지점과 반경을 배열로 선언
    TArray<FVector> SpawnOrigins = { 
         FVector(0.0f,0.0f, 1443.468667), 
         
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
     
    // 2. 랜덤하게 한 스폰 지역을 선택
    int32 RandomIndex = FMath::RandRange(0, SpawnOrigins.Num() - 1); 
    FVector OriginVector = SpawnOrigins[RandomIndex]; 
    float SearchRadius = FMath::RandRange(600.0f, SpawnRadii[RandomIndex]);  

    // 3. BB에 Origin과 Radius 저장
    BlackboardComp->SetValueAsVector(random_patrol_pos_key.SelectedKeyName, OriginVector);
    BlackboardComp->SetValueAsFloat(random_patrol_radius_key.SelectedKeyName, SearchRadius);

    return EBTNodeResult::Succeeded;
}
