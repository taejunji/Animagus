#include "BTService_CheckMovementStuck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/Pawn.h"

UBTService_CheckMovementStuck::UBTService_CheckMovementStuck()
{
    bNotifyTick = true;
    bNotifyBecomeRelevant = true;
    NodeName = TEXT("Check Movement Stuck");
}

void UBTService_CheckMovementStuck::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::OnBecomeRelevant(OwnerComp, NodeMemory);

    APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    if (Pawn)
    {
        InitialLocation = Pawn->GetActorLocation();
        ElapsedTime = 0.f;
        bInitialized = true;

        // 초기화
        OwnerComp.GetBlackboardComponent()->SetValueAsBool(bMoveStuckKey.SelectedKeyName, false);
    }
}

void UBTService_CheckMovementStuck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Character || !bInitialized) return;

    ElapsedTime += DeltaSeconds;
     
    float DistanceMoved = FVector::Dist(Character->GetActorLocation(), InitialLocation); 

    if (ElapsedTime >= TimeThreshold && Character->bIsStunned == false) 
    {
        if (DistanceMoved <= DistanceThreshold)
        {
            AAIController* AIC = OwnerComp.GetAIOwner();

            // 정체 상태임
            AIC->ClearFocus(EAIFocusPriority::Gameplay);
            Character->bUseControllerRotationYaw = false;
            Character->GetCharacterMovement()->bOrientRotationToMovement = true;
            Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
            
            OwnerComp.GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);  // Blackboard에서 타겟 제거
            OwnerComp.GetBlackboardComponent()->SetValueAsBool(can_set_target_key.SelectedKeyName, true);
            OwnerComp.GetBlackboardComponent()->SetValueAsBool(bMoveStuckKey.SelectedKeyName, true);

            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::Red,
                FString::Printf(TEXT("can_set_target_key => True "))
            );
        }
        else
        {
            // 이동 중이면 다시 초기화
            InitialLocation = Character->GetActorLocation();
            ElapsedTime = 0.f;
        }
    }
}