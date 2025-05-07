// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Defense.h"
#include "../AI/MyAIController.h"
#include "../Character/BaseCharacter.h"

UBTTaskNode_Defense::UBTTaskNode_Defense()
{
    NodeName = TEXT("Defense Skill");
}

EBTNodeResult::Type UBTTaskNode_Defense::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (Character == nullptr) return EBTNodeResult::Failed;

    // 방어
    TArray<FString> DefenseSkills = {
    TEXT("ShieldSkill"),
    TEXT("SmokeSkill")
    };

    // 랜덤하게 섞기
    for (int32 i = 0; i < DefenseSkills.Num(); ++i)
    {
        int32 RandIndex = FMath::RandRange(0, DefenseSkills.Num() - 1);
        DefenseSkills.Swap(i, RandIndex);
    }

    // 순서대로 사용 시도
    for (const FString& SkillName : DefenseSkills)
    {
        if (Character->UseSkillByName(SkillName))
        {
            UE_LOG(LogTemp, Log, TEXT("DefenseTask: Successfully used skill: %s"), *SkillName);
            return EBTNodeResult::Succeeded; // 성공하면 바로 종료
        }
    }
    return EBTNodeResult::Succeeded; // 성공하면 바로 종료


    UE_LOG(LogTemp, Warning, TEXT("DefenseTask: No available attack skill could be used."));
//    return EBTNodeResult::Failed; // 사용 가능한 스킬이 없을 경우
}
