// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_ShouldRetreat.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../AI/MyAIController.h"
#include "../Character/BaseCharacter.h"
#include "../Character/AICharacter.h"

UBTDecorator_ShouldRetreat::UBTDecorator_ShouldRetreat()
{
    NodeName = TEXT("Should Retreat Based on HP Difference");
}

bool UBTDecorator_ShouldRetreat::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return false;

    AAICharacter* SelfCharacter = Cast<AAICharacter>(AIController->GetPawn());
    if (!SelfCharacter) return false;

    UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(target_key.SelectedKeyName);
    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(TargetObject);

    if (TargetCharacter) {

        float SelfHP = SelfCharacter->GetHP();
        float TargetHP = TargetCharacter->GetHP();
        float Threshold = SelfCharacter->RetreatThreshold;

        bool shouldRun = (TargetHP - SelfHP) >= Threshold;
//        bool shouldRun = (TargetHP > SelfHP);
        if(shouldRun) UE_LOG(LogTemp, Warning, TEXT("Should Retreat => True."));

        return shouldRun;
    }

    return false;
}
