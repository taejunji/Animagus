// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Attack.h"
#include "../AI/MyAIController.h"
#include "../Character/BaseCharacter.h"

UBTTaskNode_Attack::UBTTaskNode_Attack()
{
    NodeName = TEXT("Attack Skill");
}

EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (Character == nullptr) return EBTNodeResult::Failed;

    // 공격 스킬 목록
    TArray<EAttackSkill> AttackSkills = {
        EAttackSkill::Fireball,
        EAttackSkill::MagicMissile, 
        EAttackSkill::RadialSkill, 
        EAttackSkill::ShockwaveSkill, 
        EAttackSkill::Stun 
    };

    // ShockwaveSkill을 제외한 다른 스킬들을 분리
    AttackSkills.Remove(EAttackSkill::ShockwaveSkill); 

    // 랜덤하게 섞기
    for (int32 i = 0; i < AttackSkills.Num(); ++i) 
    {
        int32 RandIndex = FMath::RandRange(0, AttackSkills.Num() - 1); 
        AttackSkills.Swap(i, RandIndex); 
    }

    // ShockwaveSkill을 가장 앞에 추가
    AttackSkills.Insert(EAttackSkill::ShockwaveSkill, 0);

    // 순서대로 사용 시도
    for (const EAttackSkill& SkillEnum : AttackSkills)
    {
        switch (SkillEnum)
        {
        case EAttackSkill::Fireball:
            if (Character->UseSkillByName(TEXT("Fireball"))) {
                return EBTNodeResult::Succeeded;
            }
            break;

        case EAttackSkill::MagicMissile:
            if (Character->UseSkillByName(TEXT("MagicMissile"))) {
                return EBTNodeResult::Succeeded;
            }
            break;

        case EAttackSkill::RadialSkill:
            if (Character->UseSkillByName(TEXT("RadialSkill"))) {
                return EBTNodeResult::Succeeded;
            }
            break;

        case EAttackSkill::ShockwaveSkill:
            if (CheckShockwaveSkill(OwnerComp, Character) && Character->UseSkillByName(TEXT("ShockwaveSkill"))) {
                UE_LOG(LogTemp, Warning, TEXT("AttackTask: Successfully used skill: ShockwaveSkill")); 
                return EBTNodeResult::Succeeded;
            }
            break;

        case EAttackSkill::Stun:
            if (Character->UseSkillByName(TEXT("Stun"))) {
                return EBTNodeResult::Succeeded;
            }
            break;

        default:
            break;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("AttackTask: No available attack skill could be used."));
    return EBTNodeResult::Failed; // 사용 가능한 스킬이 없을 경우
}

bool UBTTaskNode_Attack::CheckShockwaveSkill(UBehaviorTreeComponent& OwnerComp, ABaseCharacter* Character)
{
    auto* Target = Cast<ABaseCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(target_key.SelectedKeyName));
    if (Target == nullptr || Character == nullptr) return false;

    FVector CharacterLocation = Character->GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    float Distance = FVector::Dist(CharacterLocation, TargetLocation);

    // UE_LOG(LogTemp, Warning, TEXT("ShockwaveSkill Check: Distance = %f"), Distance); 
    // 거리 조건: 400 이하일 때만 true == 쇼크웨이브 최대 반지름이 300인데 400을 유효사거리로
    return Distance <= 400.f;
}