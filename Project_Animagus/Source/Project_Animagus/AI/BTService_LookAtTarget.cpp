// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_LookAtTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include  "../Character/BaseCharacter.h"

UBTService_LookAtTarget::UBTService_LookAtTarget()
{
    NodeName = TEXT("Look At Target");
}

void UBTService_LookAtTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (AIC == nullptr) return;

    UBlackboardComponent* BBC = OwnerComp.GetBlackboardComponent();
    if (BBC == nullptr) return;

    auto* Target = Cast<ABaseCharacter>(BBC->GetValueAsObject(target_key.SelectedKeyName));
    if (Target)
    {
        AIC->SetFocalPoint(Target->GetActorLocation() + FVector(0, 0, 50));  // 타겟의 중앙을 바라보게 설정
    }
    else
    {
        AIC->ClearFocus(EAIFocusPriority::Gameplay);  // 타겟이 없으면 초점 해제
    }
}
