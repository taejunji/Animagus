// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Skill_2.h"
#include "AIController.h"
#include "MyAIController.h"
#include "../Character/AICharacter.h"
#include "Project_Animagus/Skill/BaseSkill.h"

#include "../System/MyGameInstance.h"

#include "../Server/Server/protocol.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"

UBTTaskNode_Skill_2::UBTTaskNode_Skill_2()
{
    // 노드 이름
    NodeName = TEXT("Skill_2");
}

EBTNodeResult::Type UBTTaskNode_Skill_2::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    if (OwnerComp.GetAIOwner() == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    ABaseCharacter* Character = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
    if (Character == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    if (Character && Character->Skills.IsValidIndex(2) && Character->Skills[2])
    {
        UBaseSkill* Skill = Character->Skills[2];
        FRotator Rotation;

        if (AMyAIController* AIController = Cast<AMyAIController>(Character->GetController()))
        {
            ABaseCharacter* TargetCharacter = nullptr;
            UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
            if (BBComp && AIController->TargetKey.SelectedKeyName.IsValid())
            {
                TargetCharacter = Cast<ABaseCharacter>(BBComp->GetValueAsObject(AIController->TargetKey.SelectedKeyName));
            }
            if (TargetCharacter)
            {
                FVector DirectionToTarget = (TargetCharacter->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
                Rotation = DirectionToTarget.Rotation();
            }
            else
            {
                // 타겟이 없다면 AI Panw이 바라보는 방향으로 발사
                Rotation = Character->GetActorRotation();
            }
        }

        Skill->SetSkillRotation(Rotation.Pitch, Rotation.Yaw, Rotation.Roll);
        Skill->ActiveSkill();

        Protocol::CS_AI_USING_SKILL_PKT SkillPkt;
        SkillPkt.ai_id = Character->GetPlayerId();
        SkillPkt.s_type = Skill->SkillType;
        //SkillPkt.x = Location.X; SkillPkt.y = Location.Y; SkillPkt.z = Location.Z;  // 필수인가?
        SkillPkt.pitch = Rotation.Pitch; SkillPkt.yaw = Rotation.Yaw; SkillPkt.roll = Rotation.Roll;

        SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(SkillPkt);
        Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);

    }


    return EBTNodeResult::Succeeded;
}