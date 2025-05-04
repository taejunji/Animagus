// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TurnToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

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

    // ✅ 보간 회전 중에는 자동 회전 관련 플래그 끔
    Character->bUseControllerRotationYaw = false;
    Character->GetCharacterMovement()->bOrientRotationToMovement = false;
    Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;

    FRotator TargetRotation = (Target->GetActorLocation() - Character->GetActorLocation()).Rotation(); 
    TargetRotation.Pitch = 0.0f; // Y 축 회전(상하 방향) 무시 - Y축을 기준으로 앞뒤로 고개를 흔드는(상하로 기울어지는) 회전



    float YawDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(Character->GetActorRotation().Yaw, TargetRotation.Yaw));
    // FindDeltaAngleDegrees: 현재 Yaw(바라보고 있는 방향)와 목표 Yaw 간의 각도 차이를 계산합니다. 이 함수는 + 180 ~- 180도 범위에서 가장 짧은 회전 거리(ΔYaw)를 반환
    // ✅ 현재 Yaw와 목표 Yaw 사이의 최소 회전 각도(±180도 범위)를 반환해.
    // 예: 현재 10도, 목표 350도 → - 20도(왼쪽으로 20도 회전이 가장 짧은 거리)
    
    // float InterpSpeed = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 180.f), FVector2D(35.f, 35.f), YawDifference);
    // ✅ 특정 범위의 값을 다른 범위로 선형 매핑해주는 함수야.
    //  200도든 360도든 결과는 50.0f로 동일

    // FRotator NewRotation = FMath::RInterpTo(Character->GetActorRotation(), TargetRotation, DeltaSeconds, InterpSpeed);
    // Character->SetActorRotation(NewRotation);

    FRotator NewRotation = FMath::RInterpTo(Character->GetActorRotation(), TargetRotation, DeltaSeconds, 35.f);
    Character->SetActorRotation(NewRotation);

    // 회전 완료 체크
    if (YawDifference < 3.0f)
    {
         // ✅ 회전 완료 후 자동 회전 설정 다시 복구
        Character->bUseControllerRotationYaw = true;
        Character->GetCharacterMovement()->bOrientRotationToMovement = false;
        Character->GetCharacterMovement()->bUseControllerDesiredRotation = false; // 보통 이건 계속 false 유지하는 경우 많음
        
        b_is_turnning = false;
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); // 회전이 끝나면 다음 Task 실행
    }
}
