// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckDeadZone.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AI/MyAIController.h"
#include "GameFramework/Actor.h"

UBTService_CheckDeadZone::UBTService_CheckDeadZone()
{
	NodeName = TEXT("Check Dead Zone");
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;

	// 기본값 설정
	CenterXY = FVector::ZeroVector;
	AllowedXYRange = 2425.f;
	AllowedZDifference = 500.f;
}

void UBTService_CheckDeadZone::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	APawn* AIPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!AIPawn) return;

	const FVector MyLoc = AIPawn->GetActorLocation();

	// 1. 사각형 범위 검사 (0,0 기준으로 X,Y가 각각 ±2450 이내)
//	if (FMath::Abs(MyLoc.X) > 2450.f || FMath::Abs(MyLoc.Y) > 2450.f || MyLoc.Z > 450.f)
    if (FMath::Abs(MyLoc.X) <= 2450.f && FMath::Abs(MyLoc.Y) <= 2450.f && MyLoc.Z <= 450.f)
	{
        AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(target_key.SelectedKeyName));
        if (!TargetActor) return;

        const float ZDiff = FMath::Abs(TargetActor->GetActorLocation().Z - MyLoc.Z);

        // 3. Z차이가 500 초과하면 TargetKey 제거
        if (ZDiff > AllowedZDifference)
        {
            if (AMyAIController* AIC = Cast<AMyAIController>(OwnerComp.GetAIOwner()))
            {
                //GEngine->AddOnScreenDebugMessage(
                //    -1,
                //    2.0f,
                //    FColor::Red,
                //    FString::Printf(TEXT("데드존 -> 타겟과의 거리가 500을 넘어감 - 타겟 해제 => True "))
                //);

                AIC->ClearFocusTarget();
            }
        }
	}	
}
