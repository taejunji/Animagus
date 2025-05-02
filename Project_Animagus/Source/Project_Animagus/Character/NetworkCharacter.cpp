// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkCharacter.h"

#include "Project_Animagus/System/MyGameInstance.h"


void ANetworkCharacter::BeginPlay()
{
    Super::BeginPlay(); 
}

void ANetworkCharacter::InitPlayerMesh()
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GetGameInstance()))
    {
        switch (GameInstance->player_data.stored_mesh)
        {

        //case CharacterMesh::Tiger:
        //    if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Tiger"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Tiger")]);
        //    break;

        case CharacterMesh::Monkey:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Monkey"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Monkey")]);
            break;

        case CharacterMesh::Koala:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Koala"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Koala")]);
            break;

        case CharacterMesh::Sheep:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Sheep"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Sheep")]);
            break;

        case CharacterMesh::Fox:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Fox"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Fox")]);
            break;

        case CharacterMesh::Sloth:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Sloth"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Sloth")]);
            break;

        case CharacterMesh::Elephant:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Elephant"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Elephant")]);
            break;

        case CharacterMesh::Raccoon:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Raccoon"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Raccoon")]);
            break;

        case CharacterMesh::Deer:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Deer"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Deer")]);
            break;

        case CharacterMesh::Cow:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Cow"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Cow")]);
            break;

        case CharacterMesh::Unicorn:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Unicorn"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Unicorn")]);
            break;

        case CharacterMesh::Zebra:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Zebra"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Zebra")]);
            break;

        case CharacterMesh::Donkey:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Donkey"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Donkey")]);
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