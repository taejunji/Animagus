// Fill out your copyright notice in the Description page of Project Settings.


#include "ConnectGameMode.h"
#include "../GameMode/ConnectGameMode.h"
#include "../UI/MeshSelectWidget.h"
#include "../Actor/SkeletalPreviewActor.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/PlayerController/ConnectPlayerController.h"

void AConnectGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // PreviewActor 스폰 (GameMode가 담당)
    if (PreviewActorClass)
    {
        PreviewActor = GetWorld()->SpawnActor<ASkeletalPreviewActor>(
            PreviewActorClass,
            PreviewSpawnLocation,
            PreviewSpawnRotation
        );
        
    }
}

void AConnectGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (APawn* P = NewPlayer->GetPawn())
    {
        
        P->SetActorLocationAndRotation(SpawnLocation, SpawnRotation, false, nullptr, ETeleportType::TeleportPhysics);
        
        NewPlayer->SetControlRotation(SpawnRotation);
    }
    
}

