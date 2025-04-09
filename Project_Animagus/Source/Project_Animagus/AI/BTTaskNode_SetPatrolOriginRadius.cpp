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
        //FVector(0.f, 0.f, 0.f), 
        FVector(0.0f,0.0f, 1443.468667)
    };

    TArray<float> SpawnRadii = {
        //1300.f,  
        1300.f,

    };
     
    // 2. 랜덤하게 한 스폰 지역을 선택
    int32 RandomIndex = FMath::RandRange(0, SpawnOrigins.Num() - 1); 
    FVector OriginVector = SpawnOrigins[RandomIndex]; 
    float SearchRadius = FMath::RandRange(100.0f, SpawnRadii[RandomIndex]);  

    // 3. BB에 Origin과 Radius 저장
    BlackboardComp->SetValueAsVector(random_patrol_pos_key.SelectedKeyName, OriginVector);
    BlackboardComp->SetValueAsFloat(random_patrol_radius_key.SelectedKeyName, SearchRadius);

    // 최종 값 로그 출력
    // UE_LOG(LogTemp, Warning, TEXT("Selected Origin: X=%.2f, Y=%.2f, Z=%.2f"), OriginVector.X, OriginVector.Y, OriginVector.Z);
    // UE_LOG(LogTemp, Warning, TEXT("Selected SearchRadius: %.2f"), SearchRadius);


    return EBTNodeResult::Succeeded;
}
