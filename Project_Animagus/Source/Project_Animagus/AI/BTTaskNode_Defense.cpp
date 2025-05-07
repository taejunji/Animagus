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
    TArray<EDefenseSkill> DefenseSkills = {
        EDefenseSkill::Shield    
    };

    // 랜덤하게 섞기
    for (int32 i = 0; i < DefenseSkills.Num(); ++i)
    {
        int32 RandIndex = FMath::RandRange(0, DefenseSkills.Num() - 1);
        DefenseSkills.Swap(i, RandIndex);
    }

    // 순서대로 사용 시도
    for (const EDefenseSkill& SkillEnum : DefenseSkills)
    {
        switch (SkillEnum)
        {
        case EDefenseSkill::Shield:
            if (CheckShieldSkill(OwnerComp, Character) && Character->UseSkillByName(TEXT("ShieldSkill"))) {
                UE_LOG(LogTemp, Warning, TEXT("ShieldSkill used."));
                return EBTNodeResult::Succeeded;
            }
            break;

        default:
            break;
        }
    }

    return EBTNodeResult::Failed; // 사용 가능한 스킬이 없을 경우
}

bool UBTTaskNode_Defense::CheckShieldSkill(UBehaviorTreeComponent& OwnerComp, ABaseCharacter* Character)
{
    auto* Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(target_key.SelectedKeyName));
    if (Target == nullptr || Character == nullptr) return false;

    FVector CharacterLocation = Character->GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    float Distance = FVector::Dist(CharacterLocation, TargetLocation);

    // 거리 조건: 15m 이하일 때만 true == 유효 사거리 
    return Distance <= 1500.f;
}
