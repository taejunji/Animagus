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

    PlayerInfo = new Protocol::PlayerInfo();
    DestInfo = new Protocol::PlayerInfo();
}

ANetworkCharacter::~ANetworkCharacter()
{
    delete PlayerInfo;
    delete DestInfo;
    PlayerInfo = nullptr;
    DestInfo = nullptr;
}

void ANetworkCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    {
        FVector Location = GetActorLocation();
        PlayerInfo->x = Location.X;
        PlayerInfo->y = Location.Y;
        PlayerInfo->z = Location.Z;
        PlayerInfo->rotation = GetActorRotation().Yaw;
    }

    const Protocol::PlayerState State = GetMoveState();

    if (State == Protocol::PlayerState::MOVE_STATE_RUN)
    {
        SetActorRotation(FRotator(0, DestInfo->rotation, 0));
        AddMovementInput(GetActorForwardVector());
    }
    else
    {

    }
}

void ANetworkCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitPlayerMesh();
    GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -55), FRotator(0, -90, 0)); // 메쉬 기본 위치, 회전값 설정( X축을 앞으로 바라보도록 설정하기 위함 )
    
    SetPawnType(PawnType::NETWORK);

    FVector Location = GetActorLocation();
    DestInfo->x = Location.X;
    DestInfo->y = Location.Y;
    DestInfo->z = Location.Z;
    DestInfo->rotation = GetActorRotation().Yaw;

}

void ANetworkCharacter::SetPlayerInfo(Protocol::PlayerInfo& info)
{
    PlayerInfo->x = info.x;
    PlayerInfo->y = info.y;
    PlayerInfo->z = info.z;
    PlayerInfo->rotation = info.rotation;
    PlayerInfo->player_state = info.player_state;
}

void ANetworkCharacter::SetDestInfo(Protocol::PlayerInfo& info)
{

}

void ANetworkCharacter::InitPlayerMesh()
{
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
