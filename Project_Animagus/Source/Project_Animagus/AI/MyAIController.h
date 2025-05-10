// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h" // 필요: Blackboard 컴포넌트를 포함
#include "MyAIController.generated.h"

// 아오
// Git에서 Merge하고 충돌하면 -> Git Desktop에서 update from main해야된다
// 안그러면 바로 덮어씌워져서 꼬여버린다.
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
    
    AIControlMode ControlMode;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector IsRunningKey;

    // AI State를 저장할 Blackboard Key
    //UPROPERTY(EditAnywhere, Category = "Blackboard")
    //FBlackboardKeySelector AIStateKey;

public:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector BoxTargetKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector ItemTargetKey;

public:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector DefendRadiusKey;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector AttackRadiusKey;

public:
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector Skill_1_CoolTime_Key;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector Skill_2_CoolTime_Key;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector Skill_3_CoolTime_Key;
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector Skill_4_CoolTime_Key;

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    TArray<FBlackboardKeySelector> Skill_isCoolTime_Key;
public:

    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector patrol_pos_key;

    bool bFailedToFindNavMesh;
    
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector can_set_target_key;

    FTimerHandle TargetChangeTimerHandle; // 타겟 변경 타이머

    UPROPERTY(VisibleAnywhere)
    class UAIPerceptionComponent* AIPerceptionComponent;

    TSet<AActor*> SensedActors; // 현재 감지된 액터 목록

public:
    UFUNCTION(BlueprintCallable)
    void ResumeBehaviorTree(); // BT 재개

    void StartBehaviorTree();

    void SetControlMode(AIControlMode mode);

    void SetSkillCoolTime();

public:
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    void CheckDisableTarget();
    
    struct FAIStimulus CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense);
    
    void HandleSensedSight(AActor* Actor, bool bSensed, FAIStimulus Stimulus);
   
    class ABaseCharacter* SelectBestTarget(const TSet<AActor*>& Candidates);

    void SelectBestItemTarget(const TSet<AActor*>& Candidates);

    void SelectBestBoxTarget(const TSet<AActor*>& Candidates);

    float CalculateTargetPriority(class ABaseCharacter* TargetCharacter);    
    
    void SetAITarget(ABaseCharacter* NewTarget);
 
    void ResetTargetChange();

    void ClearFocusTarget();

protected:
    const float MOVE_PACKET_SEND_DELAY = 1.f / 30.f;
    float MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

    // Cache
    FVector2D DesiredInput;
    FVector DesiredMoveDirection;
    float DesiredYaw;

    // Dirty Flag Test
    FVector2D LastDesiredInput;

    // NavMesh 복귀 
    void CheckAndRecoverFromNavMesh();

    void CheckFindPathFromNavMesh();

    void CheckSkillCoolTime(ABaseCharacter* AI);

    void SetAIRunSpeed(ABaseCharacter* AI, float DeltaTime);

    void SetStaticActorRotation(float DeltaTime);
};