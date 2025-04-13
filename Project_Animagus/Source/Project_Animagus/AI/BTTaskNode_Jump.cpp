// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Jump.h"
#include "AIController.h"
#include "../Character/AICharacter.h"


UBTTaskNode_Jump::UBTTaskNode_Jump()
{
    // 노드 이름
    NodeName = TEXT("Jump");
}

EBTNodeResult::Type UBTTaskNode_Jump::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    if (OwnerComp.GetAIOwner() == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (Character == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    Character->Jump();

    return EBTNodeResult::Succeeded;
}