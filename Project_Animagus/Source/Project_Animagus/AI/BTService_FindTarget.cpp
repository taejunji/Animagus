// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindTarget.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/BaseCharacter.h"
#include "Engine/OverlapResult.h"

UBTService_FindTarget::UBTService_FindTarget()
{
    NodeName = TEXT("Find Target Service");
    Interval = 0.5f; // 주기
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // 주인 가져오기
    APawn* LocalPawn = OwnerComp.GetAIOwner()->GetPawn();
    if (LocalPawn == nullptr)
    {
        return;
    }

    UWorld* World = LocalPawn->GetWorld();
    if (World == nullptr)
    {
        return;
    }

    FVector Location = LocalPawn->GetActorLocation();
    // TargetKey가 있는지 확인
    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(target_key.SelectedKeyName));
    bool bHasTarget = (TargetCharacter != nullptr);

    // 색상 결정 (빨강: 타겟 있음, 파랑: 타겟 없음)
    FColor DebugColor = bHasTarget ? FColor::Red : FColor::Blue; 

    // 디버그 구체 그리기
    DrawDebugSphere(World, Location, search_radius, 16, DebugColor, false, 0.2f); 
#if 0
    TArray<FOverlapResult> OverlapResults; 
    // FOverlapResult는 하나의 오버랩 결과에 대한 정보
    
    FCollisionQueryParams CollisionQueryParam(NAME_None, false, LocalPawn);
    // 충돌 검사에 대한 파라미터를 설정
    // NAME_None: 쿼리 이름(특별한 이름을 지정하지 않으면 None으로 설정).
    // false: 충돌을 발생시킨 객체가 이 객체 자체인 경우를 무시하지 않도록
    // LocalPawn: 쿼리에서 제외할 객체를 지정

    bool bResult = World->OverlapMultiByChannel(
        OverlapResults,
        Location,
        FQuat::Identity,
        ECollisionChannel::ECC_WorldDynamic, // ECC_WorldDynamic: 동적 월드 객체들과 충돌을 검사합니다. (예: 움직이는 Actor들)
        FCollisionShape::MakeSphere(search_radius),
        CollisionQueryParam
    );

    if (bResult)
    {
        for (FOverlapResult& OverlapResult : OverlapResults)
        {
            ABaseCharacter* character = Cast<ABaseCharacter>(OverlapResult.GetActor());
            if (character && character->GetIsDead() == false)
            {
                FVector ActorLocation = character->GetActorLocation(); 

                // Z값이 특정 범위를 벗어나면 타겟 설정하지 않음
                if (FMath::Abs(ActorLocation.Z - Location.Z) <= max_z_difference) // maxZDifference는 허용하는 Z 범위
                {
                    DrawDebugSphere(World, Location, search_radius, 16, FColor::Red, false, 0.2f);
                    OwnerComp.GetBlackboardComponent()->SetValueAsObject(target_key.SelectedKeyName, character);
                    return;
                }
            }
        }
    }

    OwnerComp.GetBlackboardComponent()->SetValueAsObject(target_key.SelectedKeyName, nullptr);
    DrawDebugSphere(World, Location, search_radius, 16, FColor::Blue, false, 0.2f);
#endif
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