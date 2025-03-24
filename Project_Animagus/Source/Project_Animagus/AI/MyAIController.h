// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h" // 필요: Blackboard 컴포넌트를 포함
#include "MyAIController.generated.h"

/*
    Behavior Tree Node 
    1. Composite(복합) : 여러 개의 자식 노드 가짐
        - Selector : 자식 노드 중 하나라도 성공하면 전체 성공 [ Action 1 또는 Action 2 ]
        - Sequence : 모든 자식이 성공해야 전체 성공          [ Action 1 -> Action 2 ]
        - Simple Parallel : 동시에 실행 가능( 이동 & 공격 같이 수행 ) [ Action 1 + Action 2 ]

    2. Task(작업) : 실제 행동 수행
    3. Decorator & Service(추가기능) : 실행 조건 & 지속적 업데이트
*/

/**
 * 1. BTTask : 일 노드 -> 실패, 성공, 진행 등의 반환값 
 * 2. BTService : 반복 노드 -> "근처 적 탐색해서 블랙보드에 저장"
 * 3. BTDecorator : 실행 조건 -> "체력이 50% 이하일 때 실행"

    - Succeeded : 성공
    - Failed : 실패
    - Aborted : 중지 
    - InProgress : 진행 중 

 */

enum class AIControlMode { AIController, BehaviorTree };

//UENUM(BlueprintType) 
//enum class EAIState : uint8 
//{
//    Patrol     UMETA(DisplayName = "Patrol"),
//    Attacking  UMETA(DisplayName = "Attacking"),
//    Stun       UMETA(DisplayName = "Stun"),
//    Dead       UMETA(DisplayName = "Dead")
//};

UENUM(BlueprintType)
enum class EAIPerceptionSense : uint8
{
    EPS_Sight     UMETA(DisplayName = "Sight"),
    EPS_Hearing  UMETA(DisplayName = "Hearing"),
    EPS_Damage       UMETA(DisplayName = "Damage"),
};

UCLASS()
class PROJECT_ANIMAGUS_API AMyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMyAIController(const FObjectInitializer& ObjectInitializer);

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, Category = "AI")
    TObjectPtr<class UBehaviorTree> AIBehavior;

    UPROPERTY(EditAnywhere, Category = "AI")
    TObjectPtr<class UBlackboardData> BlackboardData;
    

public:
    void StartBehaviorTree();

    AIControlMode ControlMode;

    void SetControlMode(AIControlMode mode);

    UFUNCTION(BlueprintCallable)
    void ResumeBehaviorTree(); // BT 재개

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsRunningKey;

    // AI State를 저장할 Blackboard Key
    //UPROPERTY(EditAnywhere, Category = "Blackboard")
    //FBlackboardKeySelector AIStateKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    void CheckAndDisableTargetIfDead();

    // AI Perception Component
    UPROPERTY(VisibleAnywhere)
    class UAIPerceptionComponent* AIPerceptionComponent;

    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    struct FAIStimulus CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense);
    void HandleSensedSight(AActor* Actor, bool bSensed);

    TSet<AActor*> SensedActors; // 현재 감지된 액터 목록
};