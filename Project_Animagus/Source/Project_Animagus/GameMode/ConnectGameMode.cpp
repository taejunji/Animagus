// Fill out your copyright notice in the Description page of Project Settings.


#include "ConnectGameMode.h"
#include "../GameMode/ConnectGameMode.h"
#include "../UI/MeshSelectWidget.h"
#include "../Actor/SkeletalPreviewActor.h"
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

