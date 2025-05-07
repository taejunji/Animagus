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
    TArray<EUtilitySkill> UtilitySkills = {
        EUtilitySkill::Haste,
        EUtilitySkill::SmokeSkill,
        EUtilitySkill::ChangeSkill
    };

    UtilitySkills.Remove(EUtilitySkill::Haste);

    // 랜덤하게 섞기
    for (int32 i = 0; i < UtilitySkills.Num(); ++i)
    {
        int32 RandIndex = FMath::RandRange(0, UtilitySkills.Num() - 1);
        UtilitySkills.Swap(i, RandIndex);
    }

    UtilitySkills.Insert(EUtilitySkill::Haste, 0);

    // 순서대로 사용 시도
    for (const EUtilitySkill& SkillEnum : UtilitySkills)
    {
        switch (SkillEnum)
        {
        case EUtilitySkill::Haste:
            if (Character->UseSkillByName(TEXT("Haste"))) {
                return EBTNodeResult::Succeeded; 
            }
            break;

        case EUtilitySkill::SmokeSkill:
            if (Character->UseSkillByName(TEXT("SmokeSkill"))) {
                return EBTNodeResult::Succeeded; 
            }
            break;

        case EUtilitySkill::ChangeSkill: 
            if (Character->UseSkillByName(TEXT("ChangeSkill"))) { 
                return EBTNodeResult::Succeeded; 
            }
            break;

        default:
            break;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("UtilityTask: No available attack skill could be used."));
    return EBTNodeResult::Failed; // 사용 가능한 스킬이 없을 경우
}