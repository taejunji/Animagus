// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Utility.h"
#include "../AI/MyAIController.h"
#include "../Character/BaseCharacter.h"

UBTTaskNode_Utility::UBTTaskNode_Utility()
{
    NodeName = TEXT("Utility Skill");
}

EBTNodeResult::Type UBTTaskNode_Utility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (Character == nullptr) return EBTNodeResult::Failed;

    // 유틸
    TArray<FString> UtilitySkills = {
        TEXT("ChangeSkill"),
        TEXT("Haste")
    };

    // 랜덤하게 섞기
    for (int32 i = 0; i < UtilitySkills.Num(); ++i)
    {
        int32 RandIndex = FMath::RandRange(0, UtilitySkills.Num() - 1);
        UtilitySkills.Swap(i, RandIndex);
    }

    // 순서대로 사용 시도
    for (const FString& SkillName : UtilitySkills)
    {
        if (Character->UseSkillByName(SkillName))
        {
            UE_LOG(LogTemp, Log, TEXT("UtilityTask: Successfully used skill: %s"), *SkillName);
            return EBTNodeResult::Succeeded; // 성공하면 바로 종료
        }
    }
    return EBTNodeResult::Succeeded; // 성공하면 바로 종료

    UE_LOG(LogTemp, Warning, TEXT("UtilityTask: No available attack skill could be used."));
//    return EBTNodeResult::Failed; // 사용 가능한 스킬이 없을 경우
}

