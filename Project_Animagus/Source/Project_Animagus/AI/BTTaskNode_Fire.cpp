// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Fire.h"
#include "AIController.h"
#include "../Character/AICharacter.h"
#include "Project_Animagus/Skill/BaseSkill.h"


UBTTaskNode_Fire::UBTTaskNode_Fire()
{
    // 노드 이름
    NodeName = TEXT("Fire");
}

EBTNodeResult::Type UBTTaskNode_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
      
    if (Character && Character->Skills.IsValidIndex(0) && Character->Skills[0])
    {
        Character->Skills[0]->ActiveSkill();
    }



    return EBTNodeResult::Succeeded;
}
