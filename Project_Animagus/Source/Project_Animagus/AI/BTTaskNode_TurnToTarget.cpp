// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TurnToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UBTTaskNode_TurnToTarget::UBTTaskNode_TurnToTarget()
{
    NodeName = TEXT("Turn To Target");
    bNotifyTick = true;  // TickTask 활성화
}

EBTNodeResult::Type UBTTaskNode_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (AIC == nullptr) return EBTNodeResult::Failed; 

    auto* Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(target_key.SelectedKeyName));
    if (Target == nullptr) return EBTNodeResult::Failed; 

    b_is_turnning = true;
    return EBTNodeResult::InProgress;
}

void UBTTaskNode_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner(); 
    if (!AIController) return;

    ABaseCharacter* Character = Cast<ABaseCharacter>(AIController->GetPawn()); 
    if (!Character) return; 

    ABaseCharacter* Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(target_key.SelectedKeyName));
    if (!Target) return;

    FRotator TargetRotation = (Target->GetActorLocation() - Character->GetActorLocation()).Rotation(); 
    FRotator NewRotation = FMath::RInterpTo(Character->GetActorRotation(), TargetRotation, DeltaSeconds, 5.0f); 

    Character->SetActorRotation(NewRotation);

    // 회전 완료 체크
    // FindDeltaAngleDegrees: 현재 Yaw(바라보고 있는 방향)와 목표 Yaw 간의 각도 차이를 계산합니다. 이 함수는 + 180 ~- 180도 범위에서 가장 짧은 회전 거리(ΔYaw)를 반환
    float YawDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(Character->GetActorRotation().Yaw, TargetRotation.Yaw));
    if (YawDifference < 3.0f)
    {
        b_is_turnning = false;
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 회전이 끝나면 다음 Task 실행
    }
}
