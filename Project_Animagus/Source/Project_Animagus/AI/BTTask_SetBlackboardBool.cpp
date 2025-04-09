// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetBlackboardBool.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetBlackboardBool::UBTTask_SetBlackboardBool()
{
    // 노드 이름
    NodeName = TEXT("Set BB bool");
}

EBTNodeResult::Type UBTTask_SetBlackboardBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, bValueToSet);
    return EBTNodeResult::Succeeded;
}
