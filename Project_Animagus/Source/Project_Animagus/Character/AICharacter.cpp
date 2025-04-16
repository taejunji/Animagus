// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "../AI/MyAIController.h"
#include "Project_Animagus/System/MyGameInstance.h"
#include "../System/MyGameInstance.h"


void AAICharacter::BeginPlay()
{
    Super::BeginPlay();

    InitPlayerMesh();
    
    // 생성자에서 메쉬를 초기화했을 때는 문제가 생겼음 -> GameInstance가 초기화되지 않았는데 포인터 접근해서 프로젝트가 터짐 -> 게임 시작 시( BeginPlay )에 메쉬를 설정하도록 수정
    // 에디터에서 메쉬가 없는것처럼 보이지만 정상동작이다.

    // InitPlayerMesh(CharacterMesh::Fox);
    // GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -55), FRotator(0, -90, 0)); // 메쉬 기본 위치, 회전값 설정( X축을 앞으로 바라보도록 설정하기 위함 )

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
