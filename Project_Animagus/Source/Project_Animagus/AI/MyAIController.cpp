// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "../GameMode/BattleGameMode.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "NavigationSystem.h"

#include "Navigation/PathFollowingComponent.h"
#include "../Character/BaseCharacter.h"
#include "../Character/AICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Skill/BaseSkill.h"
#include "TimerManager.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"  // 시각 센서 추가
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AIPerceptionSystem.h"

#include "../System/MyGameInstance.h"

#include "../Server/Server/protocol.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"

#include "../Actor/ItemBox/Item_Box_Base.h"
#include "../Item/BaseItem.h"
#include "../Actor/Zones/ShrinkingZone.h"

AMyAIController::AMyAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true; // Tick 활성화

    // BehaviorTree 애셋 로드
    static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/WorkFolder/AI/AIPlayer/BT_AIPlayer.BT_AIPlayer"));
    if (BTAsset.Succeeded()) 
    {
        AIBehavior = BTAsset.Object; 
    }

    // Blackboard 애셋 로드
    static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAsset(TEXT("/Game/WorkFolder/AI/AIPlayer/BB_AIPlayer.BB_AIPlayer"));
    if (BBAsset.Succeeded()) 
    {
        BlackboardData = BBAsset.Object;
    }

    // Perception Component 생성 및 초기화
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

}

void AMyAIController::BeginPlay()
{
    Super::BeginPlay();

    // 시작하자마자 Behavior Tree 활성화 -> 레벨에 배치하자마자 실행됨
    if (AIBehavior && BlackboardData)
    {
        UBlackboardComponent* BlackboardPtr = Blackboard.Get();

        if (UseBlackboard(BlackboardData, BlackboardPtr))
        {
            // 실제 Blackboard Asset에 IsRunning이라는 키가 있어야 한다.
            IsRunningKey.SelectedKeyName = FName(TEXT("IsRunning"));
            BlackboardPtr->SetValueAsBool(IsRunningKey.SelectedKeyName, true);

            // "AIState" 설정
            // AIStateKey.SelectedKeyName = FName(TEXT("AIState"));
            // BlackboardPtr->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EAIState::Patrol));

            can_set_target_key.SelectedKeyName = FName(TEXT("CanSetTarget"));
            BlackboardPtr->SetValueAsBool(can_set_target_key.SelectedKeyName, true);

            ItemTargetKey.SelectedKeyName = FName(TEXT("ItemTarget"));
            BlackboardPtr->SetValueAsObject(ItemTargetKey.SelectedKeyName, nullptr);

            BoxTargetKey.SelectedKeyName = FName(TEXT("BoxTarget"));
            BlackboardPtr->SetValueAsObject(BoxTargetKey.SelectedKeyName, nullptr);

            DefendRadiusKey.SelectedKeyName = FName(TEXT("DefendRadius"));
            BlackboardPtr->SetValueAsFloat(DefendRadiusKey.SelectedKeyName, 600.f); // 350

            AttackRadiusKey.SelectedKeyName = FName(TEXT("AttackRadius"));
            BlackboardPtr->SetValueAsFloat(AttackRadiusKey.SelectedKeyName, 600.f);

            Skill_isCoolTime_Key.SetNum(4);
            Skill_isCoolTime_Key[0].SelectedKeyName = FName(TEXT("Skill_0_Ready"));
            Skill_isCoolTime_Key[1].SelectedKeyName = FName(TEXT("Skill_1_Ready"));
            Skill_isCoolTime_Key[2].SelectedKeyName = FName(TEXT("Skill_2_Ready"));
            Skill_isCoolTime_Key[3].SelectedKeyName = FName(TEXT("Skill_3_Ready"));

            patrol_pos_key.SelectedKeyName = FName(TEXT("PatrolLocation"));
        }

        if (AIPerceptionComponent) 
        {
            TargetKey.SelectedKeyName = FName(TEXT("Target"));
            BlackboardPtr->SetValueAsObject(TargetKey.SelectedKeyName, nullptr);

            AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AMyAIController::OnPerceptionUpdated);
        }
    }
#if 0
    StartBehaviorTree(); 
#endif

    // -> 경로 출력
    //FAIMoveRequest MoveRequest;
    //MoveRequest.SetGoalLocation(FVector(0, 0, 0));
    //MoveRequest.SetAcceptanceRadius(15.f);
    //FNavPathSharedPtr NavPath;
    //MoveTo(MoveRequest, OUT& NavPath);
    //if (NavPath.IsValid())
    //{
    //    TArray<FNavPathPoint>& PathPoints = NavPath->GetPathPoints();
    //    for (const auto& Point : PathPoints)
    //    {
    //        const FVector& Location = Point.Location;
    //        DrawDebugSphere(GetWorld(), Location, 12.f, 12, FColor::Red, false, 10.0f);
    //    }
    //}
}

void AMyAIController::StartBehaviorTree()
{
    if (AIBehavior != nullptr)
    {
        SetControlMode(AIControlMode::BehaviorTree);

        RunBehaviorTree(AIBehavior);
    }
}

void AMyAIController::SetControlMode(AIControlMode mode)
{
    switch (mode)
    {
    case AIControlMode::BehaviorTree:
        UE_LOG(LogTemp, Log, TEXT("Control Mode: BehaviorTree"));
        ControlMode = AIControlMode::BehaviorTree;
        break;

    case AIControlMode::AIController:
        UE_LOG(LogTemp, Log, TEXT("Control Mode: AIController"));
        ControlMode = AIControlMode::AIController;
        break;
    }
}

void AMyAIController::SetSkillCoolTime()
{
    //UBlackboardComponent* BlackboardPtr = Blackboard.Get();

    //if (auto* MyCharacter = Cast<ABaseCharacter>(GetPawn())) 
    //{
    //    Skill_1_CoolTime_Key.SelectedKeyName = FName(TEXT("Skill_1_CoolTime")); 
    //    BlackboardPtr->SetValueAsFloat(Skill_1_CoolTime_Key.SelectedKeyName, MyCharacter->Skills[1]->CooldownTime);

    //    Skill_2_CoolTime_Key.SelectedKeyName = FName(TEXT("Skill_2_CoolTime"));
    //    BlackboardPtr->SetValueAsFloat(Skill_2_CoolTime_Key.SelectedKeyName, MyCharacter->Skills[2]->CooldownTime);

    //    Skill_3_CoolTime_Key.SelectedKeyName = FName(TEXT("Skill_3_CoolTime"));
    //    BlackboardPtr->SetValueAsFloat(Skill_3_CoolTime_Key.SelectedKeyName, MyCharacter->Skills[3]->CooldownTime);

    //    Skill_4_CoolTime_Key.SelectedKeyName = FName(TEXT("Skill_4_CoolTime"));
    //    BlackboardPtr->SetValueAsFloat(Skill_4_CoolTime_Key.SelectedKeyName, MyCharacter->Skills[4]->CooldownTime);
    //}

}

void AMyAIController::ResumeBehaviorTree()
{
    if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent))
    {
        // Behavior Tree는 루트 노드부터 다시 시작합니다. 이때 Blackboard 값은 그대로 유지
        SetControlMode(AIControlMode::BehaviorTree);
        BTComp->RestartTree(EBTRestartMode::ForceReevaluateRootNode);
    }
}

void AMyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ABaseCharacter* AI = Cast<ABaseCharacter>(GetPawn());
    if (AI == nullptr) return;
    
    if (false == AI->GetIsDead())
    {
        UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
        if (!BlackboardComponent) return;

        // 유요한 네비 경로 확인
        CheckAndRecoverFromNavMesh();

        // 유효한 네비 경로 확인
        // CheckFindPathFromNavMesh();

        // 타겟을 해제해야하는지 확인
        CheckDisableTarget();

        // 스킬 쿨타임 확인
        // CheckSkillCoolTime(AI);

        // 달리기 속도 설정
        SetAIRunSpeed(AI, DeltaTime);

        // 고정 액터 회전
        SetStaticActorRotation(DeltaTime);
    }

    // Send 판정
    bool ForceSendPacket = false;

    if (LastDesiredInput != DesiredInput)
    {
        ForceSendPacket = true;
        LastDesiredInput = DesiredInput;
    }

    MovePacketSendTimer -= DeltaTime;

    if (MovePacketSendTimer <= 0 || ForceSendPacket)
    {
        // State 설정

        if (false == AI->GetMovementComponent()->IsFalling())
        {
            AI->SetMoveState(Protocol::PlayerState::MOVE_STATE_RUN);

            if (AI->GetVelocity().Size2D() <= 0.3f)
                AI->SetMoveState(Protocol::PlayerState::MOVE_STATE_IDLE);
        }

        MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

        Protocol::CS_AI_MOVE_PKT MovePkt;

        // 현재 위치 정보
        {
            Protocol::PlayerInfo Info;
            FVector Location = AI->GetActorLocation();
            Info.x = Location.X; Info.y = Location.Y; Info.z = Location.Z;
            Info.rotation = AI->GetActorRotation().Yaw;
            Info.player_id = AI->GetPlayerId();
            //Info.player_type = AI->GetPlayerType();
            Info.player_state = AI->GetMoveState();
            Info.speed_2d = AI->GetMovementComponent()->Velocity.Size2D();
            Info.speed_z = AI->GetMovementComponent()->Velocity.Z;

            MovePkt.player_info = Info;
        }

        SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(MovePkt);
        Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
    }

}

void AMyAIController::CheckDisableTarget()
{
    // 기본 참조
    AAICharacter* AI = Cast<AAICharacter>(GetPawn());
    ABattleGameMode* BattleMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    // 1. 캐릭터 타겟 검사
    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
    if (!TargetCharacter || TargetCharacter->GetIsDead()) {
        ClearFocusTarget();
    }

    // 2. 박스 타겟 검사
    AItem_Box_Base* TargetBox = Cast<AItem_Box_Base>(Blackboard->GetValueAsObject(BoxTargetKey.SelectedKeyName));
    if (TargetBox && TargetBox->GetHp() <= 0.f) {
        if (AI) {
            AI->bUseControllerRotationYaw = false;
            AI->GetCharacterMovement()->bOrientRotationToMovement = true;
            AI->GetCharacterMovement()->bUseControllerDesiredRotation = false;
        }

        ClearFocus(EAIFocusPriority::Gameplay);
        Blackboard->ClearValue(BoxTargetKey.SelectedKeyName); 
    }

    //ABaseItem* TargetIem = Cast<ABaseItem>(Blackboard->GetValueAsObject(ItemTargetKey.SelectedKeyName));
    //if (TargetIem && TargetBox->GetHp() <= 0.f) {
    //    if (AI) {
    //        AI->bUseControllerRotationYaw = false;
    //        AI->GetCharacterMovement()->bOrientRotationToMovement = true;
    //        AI->GetCharacterMovement()->bUseControllerDesiredRotation = false;
    //    }

    //    ClearFocus(EAIFocusPriority::Gameplay);
    //    Blackboard->ClearValue(ItemTargetKey.SelectedKeyName);
    //}

    // 3. 게임 라운드에 따라 해제
    if (BattleMode && AI)
    {
        // 상시 전투 구역 -> 라운드 시간에 영향 받지 않는다 -> 타겟이 있으면 싸우고, 박스가 있으면 먹고
        // bool bInCombatZone = ((FMath::Abs(AI->GetActorLocation().X) <= 3300.f) && (FMath::Abs(AI->GetActorLocation().Y) <= 3300.f));

        if (TargetCharacter && BattleMode->ShrinkingZone) {

            float HPDifference = TargetCharacter->GetHP() - AI->GetHP();
            // float RandomAlpha = FMath::RandRange(10.f, 40.f);

            float DistanceAI = FVector::Dist(FVector::ZeroVector, AI->GetActorLocation()); 

            // ShrinkingZone이 있고, 해당 범위에 벗어나면 도망
             if (DistanceAI >= BattleMode->ShrinkingZone->CurrentRadius) {
                 if (HPDifference >= (AI->RetreatThreshold)) { 
                     UE_LOG(LogTemp, Error, TEXT("안개구역 => HP 차이가 RetreatThreshold 이상일 경우")); 
                     ClearFocusTarget(); 
                 }
             }
#if 0
            if (false == bInCombatZone) {

                // 전투 구역이 아니고, HP 차이가 RetreatThreshold + RandomAlpha 이상일 경우
                if (HPDifference >= (AI->RetreatThreshold + RandomAlpha)) {
                    UE_LOG(LogTemp, Error, TEXT("평화 구역 => HP 차이가 RetreatThreshold + RandomAlpha 이상일 경우"));
                    ClearFocusTarget();
                }
            }
            else {
                // 전투 구역 내에서 거리 계산

                float DistanceAI = FVector::Dist(FVector::ZeroVector, AI->GetActorLocation());

                // ShrinkingZone이 있고, 해당 범위에 벗어나면 도망
                if (BattleMode->ShrinkingZone && DistanceAI >= BattleMode->ShrinkingZone->CurrentRadius) {
                    if (HPDifference >= (AI->RetreatThreshold)) {
                        UE_LOG(LogTemp, Error, TEXT("전투 구역 + 안개구역 => HP 차이가 RetreatThreshold 이상일 경우"));
                        ClearFocusTarget();
                    }
                }
            }
#endif
        }

        if (TargetBox && BattleMode->ShrinkingZone) {

            float DistanceBox = FVector::Dist(FVector::ZeroVector, TargetBox->GetActorLocation());
             if (DistanceBox >= BattleMode->ShrinkingZone->CurrentRadius) {
                 UE_LOG(LogTemp, Error, TEXT("박스 위치 => 안개 존 => 박스 X"));
            
                 AI->bUseControllerRotationYaw = false;
                 AI->GetCharacterMovement()->bOrientRotationToMovement = true;
                 AI->GetCharacterMovement()->bUseControllerDesiredRotation = false;
            
                 ClearFocus(EAIFocusPriority::Gameplay);
                 Blackboard->ClearValue(BoxTargetKey.SelectedKeyName);
             }
        }
       
#if 0
        if (false == bInCombatZone && TargetBox && BattleMode->GetCurrentRoundTime() > 90.f) {
            UE_LOG(LogTemp, Error, TEXT("평화 구역 => 라운드 1분 30초 => 박스 X"));

            AI->bUseControllerRotationYaw = false;
            AI->GetCharacterMovement()->bOrientRotationToMovement = true;
            AI->GetCharacterMovement()->bUseControllerDesiredRotation = false;

            ClearFocus(EAIFocusPriority::Gameplay); 
            Blackboard->ClearValue(BoxTargetKey.SelectedKeyName); 
        }
#endif
    }
}

void AMyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    TSet<AActor*> CandidateTargets; // 후보군 타겟들을 저장할 집합 (중복 방지) 
    TSet<AActor*> CandidateItemTargets; // 후보군 타겟들을 저장할 집합 (중복 방지) 
    TSet<AActor*> CandidateBoxTargets; // 후보군 타겟들을 저장할 집합 (중복 방지) 
    FAIStimulus AIStimulus;

    for (AActor* UpdatedActor : UpdatedActors)
    {        
        // 시각 감지
        AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Sight); 
        if (AIStimulus.WasSuccessfullySensed())
        {
            if (ABaseCharacter* character = Cast<ABaseCharacter>(UpdatedActor)) {
                CandidateTargets.Add(character);
            }

            if (ABaseItem* Item = Cast<ABaseItem>(UpdatedActor)) {
                CandidateItemTargets.Add(Item);       
            }

            if (AItem_Box_Base* Box = Cast<AItem_Box_Base>(UpdatedActor)) {
                UObject* ItemTarget = GetBlackboardComponent()->GetValueAsObject(ItemTargetKey.SelectedKeyName); 
                UObject* BoxTarget = GetBlackboardComponent()->GetValueAsObject(BoxTargetKey.SelectedKeyName); 
                if (ItemTarget == nullptr && BoxTarget == nullptr) { 
                    CandidateBoxTargets.Add(Box); 
                }
            }

            // UE_LOG(LogTemp, Warning, TEXT("시야에 감지됐습니다: %s"), *UpdatedActor->GetName());
        }

        // 피해 감지
        AIStimulus = CanSenseActor(UpdatedActor, EAIPerceptionSense::EPS_Damage);
        if (AIStimulus.WasSuccessfullySensed())
        {
            if (ABaseCharacter* character = Cast<ABaseCharacter>(UpdatedActor)) {
                CandidateTargets.Add(character);
            }

            // UE_LOG(LogTemp, Warning, TEXT("타격을 감지했습니다: %s"), *UpdatedActor->GetName());
        }
    }

    SelectBestItemTarget(CandidateItemTargets);

    SelectBestBoxTarget(CandidateBoxTargets); 

    UObject* Target = GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName);
    if (nullptr == Target) GetBlackboardComponent()->SetValueAsBool(can_set_target_key.SelectedKeyName, true);

    bool bCanSet = GetBlackboardComponent()->GetValueAsBool(can_set_target_key.SelectedKeyName);

    if (bCanSet == false) return; // 타겟 변경이 불가능한 경우 나가기

    ABaseCharacter* BestTarget = SelectBestTarget(CandidateTargets); 

    SetAITarget(BestTarget); 
}

FAIStimulus AMyAIController::CanSenseActor(AActor* Actor, EAIPerceptionSense AIPerceptionSense)
{
    FActorPerceptionBlueprintInfo ActorPerceptionBlueprintInfo;
	FAIStimulus ResultStimulus;

    AIPerceptionComponent->GetActorsPerception(Actor, ActorPerceptionBlueprintInfo); 

	TSubclassOf<UAISense> QuerySenseClass;
	switch (AIPerceptionSense)
	{

	case EAIPerceptionSense::EPS_Sight:
		QuerySenseClass = UAISense_Sight::StaticClass();
		break;

	case EAIPerceptionSense::EPS_Damage: 
		QuerySenseClass = UAISense_Damage::StaticClass(); 
		break;

	default:
		break;
	}

	TSubclassOf<UAISense> LastSensedStimulusClass;

	for (const FAIStimulus& AIStimulus : ActorPerceptionBlueprintInfo.LastSensedStimuli)
	{
		LastSensedStimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, AIStimulus);

		if (QuerySenseClass == LastSensedStimulusClass)
		{
			ResultStimulus = AIStimulus;
			return ResultStimulus;
		}
	}
	return ResultStimulus;
}

void AMyAIController::HandleSensedSight(AActor* Actor, bool bSensed, FAIStimulus Stimulus)
{
    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(Actor);
    if (TargetCharacter == nullptr) return;
     
    if (TargetCharacter->GetIsDead()) 
    {
        ClearFocusTarget();  
    }
    else if (bSensed)
    {
        GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, TargetCharacter);
    }
}

float AMyAIController::CalculateTargetPriority(ABaseCharacter* TargetCharacter)
{
    const float DistanceWeight = 1.0f;
    const float HPWeight = 0.5f;
    const float AngleWeight = 10.0f; // 각도에 대한 가중치 (크게 하면 정면을 더 우선시)

    if (!IsValid(GetPawn()) || !IsValid(TargetCharacter)) return FLT_MAX;

    // 1. 거리 계산
    float Distance = FVector::Dist(GetPawn()->GetActorLocation(), TargetCharacter->GetActorLocation());

    // 2. HP 값
    float HP = TargetCharacter->GetHP();

    // 3. 각도 차이 계산
    FVector ToTarget = (TargetCharacter->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal();
    FVector Forward = GetPawn()->GetActorForwardVector();

    float Dot = FVector::DotProduct(Forward, ToTarget);
    float AngleRadians = FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f));
    // 1.0: 두 벡터가 완전히 같은 방향 (0도)
    // 0.0: 두 벡터가 직각(90도)
    // - 1.0 : 두 벡터가 완전히 반대 방향(180도)
    float AngleDegrees = FMath::RadiansToDegrees(AngleRadians); // 0도가 정면

    // 총 우선순위 = 거리 + 체력 + 각도
    return (Distance * DistanceWeight) + (HP * HPWeight) + (AngleDegrees * AngleWeight);
}

ABaseCharacter* AMyAIController::SelectBestTarget(const TSet<AActor*>& Candidates) 
{
    ABaseCharacter* BestTarget = nullptr;
    float BestPriority = FLT_MAX;

    for (AActor* Candidate : Candidates)
    {
        ABaseCharacter* CandidateCharacter = Cast<ABaseCharacter>(Candidate);
        if (!CandidateCharacter) continue;

        float CurrentPriority = CalculateTargetPriority(CandidateCharacter);
        if (CurrentPriority < BestPriority)
        {
            BestPriority = CurrentPriority;
            BestTarget = CandidateCharacter;
        }
    }

    return BestTarget;
}

void AMyAIController::SelectBestItemTarget(const TSet<AActor*>& CandidateItems)
{
    // 가장 가까운 접근 가능한 아이템 찾기
    ABaseItem* NearestItem = nullptr;
    float MinDistance = FLT_MAX;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (nullptr == NavSys) return; 

    for (AActor* Item : CandidateItems)
    {
        ABaseItem* BaseItem = Cast<ABaseItem>(Item); 
        if (!BaseItem) continue; 

        const float Distance = FVector::Dist(BaseItem->GetActorLocation(), ControlledPawn->GetActorLocation()); 

        // 현재 위치에서 목적지까지 경로가 실제로 존재하는지 "테스트"하는 함수
        // 🔁 내부적으로 A* 같은 경로 탐색을 함 🔍 "길이 막혀 있는지" 확인할 때 적합
        if (Distance < MinDistance && 
            NavSys->TestPathSync(FPathFindingQuery(this, *NavSys->GetDefaultNavDataInstance(), ControlledPawn->GetActorLocation(), BaseItem->GetActorLocation()))) 
        {
            MinDistance = Distance; 
            NearestItem = BaseItem; 
        } 
    }

    if (NearestItem)
    {
        // Blackboard 값이 새로 설정된 아이템과 다를 경우에만 업데이트
        if (NearestItem != Cast<ABaseItem>(GetBlackboardComponent()->GetValueAsObject(ItemTargetKey.SelectedKeyName))) 
        {
            GetBlackboardComponent()->SetValueAsObject(ItemTargetKey.SelectedKeyName, NearestItem);
            UE_LOG(LogTemp, Warning, TEXT("가장 가까운 접근 가능한 아이템 설정됨: %s"), *NearestItem->GetName());
        }
    }
}

void AMyAIController::SelectBestBoxTarget(const TSet<AActor*>& CandidateBoxs)
{
    // 가장 가까운 접근 가능한 아이템 찾기
    AItem_Box_Base* NearestBox = nullptr;
    float MinDistance = FLT_MAX;

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (nullptr == NavSys) return;

    for (AActor* Box : CandidateBoxs)
    {
        AItem_Box_Base* BaseBox = Cast<AItem_Box_Base>(Box);
        if (!BaseBox) continue;

        const float Distance = FVector::Dist(BaseBox->GetActorLocation(), ControlledPawn->GetActorLocation());

        if (Distance < MinDistance &&
            NavSys->TestPathSync(FPathFindingQuery(this, *NavSys->GetDefaultNavDataInstance(), ControlledPawn->GetActorLocation(), BaseBox->GetActorLocation())))
        {
            MinDistance = Distance;
            NearestBox = BaseBox;
        }
    }

    if (NearestBox)
    {
        // Blackboard 값이 새로 설정된 아이템과 다를 경우에만 업데이트
        if (NearestBox != Cast<AItem_Box_Base>(GetBlackboardComponent()->GetValueAsObject(BoxTargetKey.SelectedKeyName)))
        {
            GetBlackboardComponent()->SetValueAsObject(BoxTargetKey.SelectedKeyName, NearestBox);
            UE_LOG(LogTemp, Warning, TEXT("가장 가까운 접근 가능한 박스 설정됨: %s"), *NearestBox->GetName());
        }
    }
}

void AMyAIController::SetAITarget(ABaseCharacter* NewTarget)
{
    if (NewTarget)
    {
        // 중복 타이머 방지
        GetWorld()->GetTimerManager().ClearTimer(TargetChangeTimerHandle);

        GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, NewTarget);
        GetBlackboardComponent()->SetValueAsBool(can_set_target_key.SelectedKeyName, false);

        GetWorld()->GetTimerManager().SetTimer(TargetChangeTimerHandle, this, &AMyAIController::ResetTargetChange, 20.0f, false);
    }
    else
    {
        ClearFocusTarget();
    }
}

void AMyAIController::ResetTargetChange()
{
    GetBlackboardComponent()->SetValueAsBool(can_set_target_key.SelectedKeyName, true);
}

// 보완 필요 -> 이놈이 원흉이다
void AMyAIController::CheckAndRecoverFromNavMesh()
{
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (nullptr == NavSys) return;

    FVector CurrentLocation = GetPawn()->GetActorLocation();
    FNavLocation ProjectedLocation;

    // 월드상의 임의의 위치가 내비게이션 메시(NavMesh) 위에 있는지 확인하고, 가장 가까운 네비 메시 위의 위치를 반환
    bool bOnNavMesh = NavSys->ProjectPointToNavigation(CurrentLocation, ProjectedLocation, FVector(30.f, 30.f, 500.f));

    if (false == bOnNavMesh && false == bFailedToFindNavMesh)
    {
        FVector PatrolLoc = GetBlackboardComponent()->GetValueAsVector(patrol_pos_key.SelectedKeyName); 

        // 결과 위치에 디버그 구를 그립니다.
        // DrawDebugSphere(GetWorld(), PatrolLoc, 200.f, 12, FColor::Red, false, 0.5f);

        // 가까운 네비 위치 방향으로 걸어가게 명령
        FVector Direction = (PatrolLoc - CurrentLocation);
        Direction.Z = 0; // Z 제거
        Direction = Direction.GetSafeNormal();

        if (AAICharacter* AICharacter = Cast<AAICharacter>(GetPawn()))
        {
            AICharacter->AddMovementInput(Direction, 1.0f);
            AICharacter->JumpAI();
        }

        //GEngine->AddOnScreenDebugMessage(
        //    -1,
        //    2.0f,
        //    FColor::Red,
        //    FString::Printf(TEXT("Failed To Find NavMesh => True "))
        //);

        // 네비메시를 찾지 못한 상태를 기록
        bFailedToFindNavMesh = true;
    }
    else {
        // 디버그 구 그리기
        // DrawDebugSphere(GetWorld(), ProjectedLocation.Location, 200.f, 12, FColor::Blue, false, 0.5f);

        // 네비메시가 발견되면 다시 실패 토글을 리셋
        bFailedToFindNavMesh = false;
    }
}

void AMyAIController::CheckFindPathFromNavMesh()
{
    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));

    // NavMesh 경로 검사
    APawn* ControlledPawn = GetPawn(); 
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()); 

    if (NavSys && ControlledPawn && TargetCharacter)
    {
        FPathFindingQuery Query(this, *NavSys->GetDefaultNavDataInstance(), ControlledPawn->GetActorLocation(), TargetCharacter->GetActorLocation()); 
        FPathFindingResult Result = NavSys->FindPathSync(Query); 

        if (!Result.IsSuccessful())
        {
            // 경로가 없으면 타겟 무효화
            ClearFocusTarget();
        }
    }
}

void AMyAIController::CheckSkillCoolTime(ABaseCharacter* AI)
{
    for (int32 i = 0; i < AI->Skills.Num(); i++)
    {
        if (AI->Skills.IsValidIndex(i) && AI->Skills[i] != nullptr)
        {
            GetBlackboardComponent()->SetValueAsBool(Skill_isCoolTime_Key[i].SelectedKeyName, AI->Skills[i]->bIsCooldown);
        }
    }
}

void AMyAIController::SetAIRunSpeed(ABaseCharacter* AI, float DeltaTime)
{
    // Blackboard에서 키의 FKey를 가져옴
    FBlackboard::FKey IsRunningKeyID = GetBlackboardComponent()->GetKeyID(IsRunningKey.SelectedKeyName);

    bool bIsRunning = false;
    if (IsRunningKeyID != FBlackboard::InvalidKey) {
        bIsRunning = GetBlackboardComponent()->GetValueAsBool(IsRunningKey.SelectedKeyName);
    }

    // 목표 속도 설정 (달리기 상태에 따라 달라짐)
    float TargetSpeed = bIsRunning ? AI->default_run_speed : AI->default_walk_speed;

    // 현재 속도를 목표 속도로 점진적으로 변경
    AI->current_speed = FMath::FInterpTo(AI->current_speed, TargetSpeed, DeltaTime, AI->speed_change_rate);

    // 캐릭터의 이동 속도 업데이트
    AI->SetWalkSpeed(AI->current_speed);
}

void AMyAIController::SetStaticActorRotation(float DeltaTime)
{
    UObject* TargetObject = GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName); 
    UObject* ItemObject = GetBlackboardComponent()->GetValueAsObject(ItemTargetKey.SelectedKeyName); 
    AItem_Box_Base* TargetBox = Cast<AItem_Box_Base>(GetBlackboardComponent()->GetValueAsObject(BoxTargetKey.SelectedKeyName)); 

    if (TargetObject == nullptr && ItemObject == nullptr && TargetBox) 
    { 
        FVector TargetLocation = TargetBox->GetActorLocation(); 
        FVector AIForwardVector = GetPawn()->GetActorForwardVector(); 

        // AI의 현재 위치와 타겟 위치로부터 방향 계산
        FVector DirectionToTarget = (TargetLocation - GetPawn()->GetActorLocation()).GetSafeNormal();
        FRotator TargetRotation = DirectionToTarget.Rotation();

        // 현재 회전값
        FRotator CurrentRotation = GetPawn()->GetActorRotation();

        // 부드러운 회전을 위해 보간 (DeltaTime은 Tick 함수에서 받아올 것)
        FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.f);

        // 회전 적용
        GetPawn()->SetActorRotation(SmoothRotation);
    }
}

void AMyAIController::ClearFocusTarget()
{
    if (ABaseCharacter* AI = Cast<ABaseCharacter>(GetPawn())) {
        AI->bUseControllerRotationYaw = false;
        AI->GetCharacterMovement()->bOrientRotationToMovement = true;
        AI->GetCharacterMovement()->bUseControllerDesiredRotation = false;
    }

    ClearFocus(EAIFocusPriority::Gameplay);  // Focus 해제
    GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);
    GetBlackboardComponent()->SetValueAsBool(can_set_target_key.SelectedKeyName, true);
}
