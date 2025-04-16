// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "../AI/MyAIController.h"
#include "Project_Animagus/System/MyGameInstance.h"

void AAICharacter::BeginPlay()
{
    Super::BeginPlay();
    InitPlayerMesh();
}

void AAICharacter::ApplyStun(float Duration)
{
    // 이미 스턴 중이면 무시
    if (bIsStunned)
    {
        return;
    }

    bIsStunned = true;
    UE_LOG(LogTemp, Log, TEXT("%s is stunned for %f seconds"), *GetName(), Duration);

    // 플레이어 입력 차단 (플레이어 컨트롤러가 있다면)
    if (AAIController* PC = Cast<AAIController>(GetController()))
    {
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }

    // 타이머 설정: Duration 이후에 RemoveStun() 호출
    FTimerHandle StunTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &AAICharacter::RemoveStun, Duration, false);
}

void AAICharacter::RemoveStun()
{
    bIsStunned = false;
    UE_LOG(LogTemp, Log, TEXT("%s is no longer stunned"), *GetName());

    // 입력 복구
    if (AAIController* PC = Cast<AAIController>(GetController()))
    {
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
    }
}

void AAICharacter::InitPlayerMesh()
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GetGameInstance()))
    {
        switch (GameInstance->player_data.stored_mesh)
        {
        case CharacterMesh::Monkey:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Monkey"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Monkey")]);
            break;

        case CharacterMesh::Tiger:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Tiger"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Tiger")]);
            break;

        case CharacterMesh::Koala:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Koala"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Koala")]);
            break;

        case CharacterMesh::Sheep:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Sheep"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Sheep")]);
            break;

        default:
            UE_LOG(LogTemp, Error, TEXT("메쉬 로드 실패"));
            break;
        }
    }

    if (AuraMaterialInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("InitPlayerMesh: AuraMaterialInstance 이미 생성됨, 재설정 진행."));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("InitPlayerMesh: AuraMaterialInstance 없음, 새로 생성합니다."));
    }
    
    if (BaseAuraMaterial && GetMesh())
    {
       
        AuraMaterialInstance = UMaterialInstanceDynamic::Create(BaseAuraMaterial, this);

        if (AuraMaterialInstance)
        {
            // 초기 기본 색상 설정 
            AuraMaterialInstance->SetVectorParameterValue(FName("BaseColor"), FLinearColor::White);
            GetMesh()->SetOverlayMaterial(AuraMaterialInstance);
            UE_LOG(LogTemp, Log, TEXT("PlayerCharacter: Dynamic Aura Material Instance created and set."));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter: Failed to create dynamic aura material instance."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("aura 머티리얼 설정 실패"));
    } 
    UE_LOG(LogTemp, Log, TEXT("InitPlayerMesh() 끝"));
}
