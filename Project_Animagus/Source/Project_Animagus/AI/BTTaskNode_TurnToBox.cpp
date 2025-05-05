// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TurnToBox.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/BaseCharacter.h"
#include "../Actor/ItemBox/Item_Box_Base.h"
#include "../Item/BaseItem.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

UBTTaskNode_TurnToBox::UBTTaskNode_TurnToBox()
{
    NodeName = TEXT("Turn To Box");
    bNotifyTick = true;  // TickTask 활성화
}

EBTNodeResult::Type UBTTaskNode_TurnToBox::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (AIC == nullptr) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent(); 
    if (!BlackboardComp) return EBTNodeResult::Failed; 

    // 두 객체 중 하나라도 존재하면 회전 시작
    UObject* BoxObject = BlackboardComp->GetValueAsObject(box_target_key.SelectedKeyName);
    UObject* ItemObject = BlackboardComp->GetValueAsObject(box_target_key.SelectedKeyName);

    if (!BoxObject && !ItemObject)
    {
        return EBTNodeResult::Failed;
    }

    b_is_turnning = true;
    return EBTNodeResult::InProgress;
}

void UBTTaskNode_TurnToBox::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    ABaseCharacter* Character = Cast<ABaseCharacter>(AIController->GetPawn());
    if (!Character) return;

    // Blackboard에서 Box 또는 Item 가져오기
    AItem_Box_Base* Box = Cast<AItem_Box_Base>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(box_target_key.SelectedKeyName)); 
    ABaseItem* Item = Cast<ABaseItem>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(box_target_key.SelectedKeyName)); 

    AActor* TargetActor = nullptr; 
    float InterpSpeed;
    // 보간 속도: Item은 7, Box는 35
    if (Item) 
    {
        TargetActor = Item; 
        InterpSpeed = 7.f;
    }
    else if (Box)
    {
        TargetActor = Box; 
        InterpSpeed = 35.f;
    }

    if (!TargetActor) return;

    // ✅ 보간 회전 중에는 자동 회전 관련 플래그 끔
    Character->bUseControllerRotationYaw = false;
    Character->GetCharacterMovement()->bOrientRotationToMovement = false;
    Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;

    FRotator TargetRotation = (TargetActor->GetActorLocation() - Character->GetActorLocation()).Rotation();
    TargetRotation.Pitch = 0.0f;

    float YawDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(Character->GetActorRotation().Yaw, TargetRotation.Yaw));

    // 각도 차이가 크면 느리게(7), 작으면 빠르게(35)
    InterpSpeed = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 180.f), FVector2D(7.f, 35.f), YawDifference); 

    FRotator NewRotation = FMath::RInterpTo(Character->GetActorRotation(), TargetRotation, DeltaSeconds, InterpSpeed);
    Character->SetActorRotation(NewRotation);

    if (YawDifference < 1.0f)
    {
        Character->bUseControllerRotationYaw = true;
        Character->GetCharacterMovement()->bOrientRotationToMovement = false;
        Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;

        b_is_turnning = false;
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

