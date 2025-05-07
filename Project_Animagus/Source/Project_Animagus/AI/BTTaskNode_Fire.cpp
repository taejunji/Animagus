// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Fire.h"
#include "AIController.h"
#include "MyAIController.h"
#include "../Character/AICharacter.h"
#include "Project_Animagus/Skill/BaseSkill.h"

#include "../System/MyGameInstance.h"

#include "../Server/Server/protocol.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"


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
      
    Character->UseSkillByName(TEXT("Fireball"));

    return EBTNodeResult::Succeeded;
}
