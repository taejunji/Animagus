// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckTargetInSight.h"
#include "../AI/MyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/BaseCharacter.h"
#include "Perception/AISense_Sight.h"

UBTDecorator_CheckTargetInSight::UBTDecorator_CheckTargetInSight()
{
    NodeName = TEXT("Check Target In Sight");
}

bool UBTDecorator_CheckTargetInSight::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    AMyAIController* AIC = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (nullptr == AIC) return false;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (nullptr == BlackboardComp) return false;

    ABaseCharacter* TargetActor = Cast<ABaseCharacter>(BlackboardComp->GetValueAsObject(AIC->TargetKey.SelectedKeyName));
    if (nullptr == TargetActor) return false;

//    return IsTargetInSight(AIC, TargetActor);

    bool bResult = IsTargetInSight(AIC, TargetActor);

    //// ★ GEngine을 사용해서 화면에 출력
#if 0
    if (GEngine)
    {
        if (bResult)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,            // Key (-1 = 항상 새로 추가)
                2.0f,          // Duration (2초)
                FColor::Purple, // Text Color
                FString::Printf(TEXT("CheckTargetInSight: SUCCESS (%s)"), *TargetActor->GetName())
            );
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::Red,
                FString::Printf(TEXT("CheckTargetInSight: FAIL (%s)"), *TargetActor->GetName())
            );
        }
    }
#endif
    return bResult;
}

bool UBTDecorator_CheckTargetInSight::IsTargetInSight(AMyAIController* AIController, AActor* TargetActor) const
{
    if (!AIController || !TargetActor) return false;

    FActorPerceptionBlueprintInfo Info;
    AIController->AIPerceptionComponent->GetActorsPerception(TargetActor, Info);

    for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
    {
        TSubclassOf<UAISense> StimulusSenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(AIController, Stimulus);

        if (StimulusSenseClass == UAISense_Sight::StaticClass())
        {
            // 👇 추가 : Stimulus가 만료되었는지도 체크
            // 감지(Stimulus)가 만료돼도, Perception 시스템은 그 정보를 "기억"하고 있을 수 있어.
            if (!Stimulus.IsExpired())
            {
                return Stimulus.WasSuccessfullySensed();
            }
            else
            {
                // Stimulus AGE가 만료되었으면 인식 못하는 걸로 간주
                return false;
            }
        }
    }

    return false;
}
