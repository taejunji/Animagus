// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkCharacter.h"
#include "../System/MyGameInstance.h"
#include "../Server/Server/protocol.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


//using namespace Protocol;

ANetworkCharacter::ANetworkCharacter()
{
    // Tick 활성화
    PrimaryActorTick.bCanEverTick = true;

    // 네트워크 복제 설정
    bReplicates = true;
    bAlwaysRelevant = true;

    // 충돌영역 정의
    GetCapsuleComponent()->SetCapsuleSize(54.358692, 38.444557);
}

void ANetworkCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ANetworkCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitPlayerMesh();
    GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -55), FRotator(0, -90, 0)); // 메쉬 기본 위치, 회전값 설정( X축을 앞으로 바라보도록 설정하기 위함 )
}

void ANetworkCharacter::InitPlayerMesh()
{   // 메쉬 설정 어캐함요?
    if (auto* GameInstance = Cast<UMyGameInstance>(GetGameInstance()))
    {
        switch (PlayerType)
        {
        case Protocol::PlayerType::MONKEY:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Monkey"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Monkey")]);
            break;
        case Protocol::PlayerType::TIGER:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Tiger"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Tiger")]);
            break;
        case Protocol::PlayerType::KOALA:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Koala"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Koala")]);
            break;
        case Protocol::PlayerType::RAM:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Sheep"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Sheep")]);
            break;
        default:
            UE_LOG(LogTemp, Error, TEXT("메쉬 로드 실패"));
            break;
        }
    }
}
