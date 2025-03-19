// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_StopBehaviorTree.h"
#include "../AI/MyAIController.h"

UBTTaskNode_StopBehaviorTree::UBTTaskNode_StopBehaviorTree()
{
    NodeName = TEXT("Stop Behavior Tree");
}

EBTNodeResult::Type UBTTaskNode_StopBehaviorTree::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AMyAIController* AIC = Cast< AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIC) return EBTNodeResult::Failed;

    AIC->SetControlMode(AIControlMode::AIController);
    OwnerComp.StopTree();

    return EBTNodeResult::Succeeded;
}
