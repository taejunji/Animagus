// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Skill_1.h"
#include "AIController.h"
#include "../Character/AICharacter.h"
#include "Project_Animagus/Skill/BaseSkill.h"

UBTTaskNode_Skill_1::UBTTaskNode_Skill_1()
{
    // 노드 이름
    NodeName = TEXT("Skill_1");
}

EBTNodeResult::Type UBTTaskNode_Skill_1::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    if (Character && Character->Skills.IsValidIndex(1) && Character->Skills[1])
    {
        Character->Skills[1]->ActiveSkill();
    }


    return EBTNodeResult::Succeeded;
}
