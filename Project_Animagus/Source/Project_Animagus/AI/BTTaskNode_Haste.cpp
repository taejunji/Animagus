// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Haste.h"
#include "AIController.h"
#include "../Character/AICharacter.h"
#include "Project_Animagus/Skill/BaseSkill.h"
#include "../GameMode/BattleGameMode.h"
#include "Kismet/GameplayStatics.h"


UBTTaskNode_Haste::UBTTaskNode_Haste()
{
    // 노드 이름
    NodeName = TEXT("Haste");
}

EBTNodeResult::Type UBTTaskNode_Haste::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    if (OwnerComp.GetAIOwner() == nullptr) {
        return EBTNodeResult::Failed;
    }

    ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (Character == nullptr) {
        return EBTNodeResult::Failed;
    }

    ABattleGameMode* BM = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!BM) {
        return EBTNodeResult::Failed;
    }

    // 게임 시작 3초 전이면 수행 X
    if (BM->GetCurrentRoundTime() < 3.f) {
        return EBTNodeResult::Succeeded;
    }

    bool Success = Character->UseSkillByName(TEXT("Haste"));
    if(Success) UE_LOG(LogTemp, Warning, TEXT("Haste: Successfully used skill"));

    return EBTNodeResult::Succeeded;
}

