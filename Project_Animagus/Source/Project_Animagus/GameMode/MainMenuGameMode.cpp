// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "../PlayerController/MainMenuController.h"
#include "Project_Animagus/System/MyGameInstance.h"

AMainMenuGameMode::AMainMenuGameMode()
{
    
    PlayerControllerClass = AMainMenuController::StaticClass();
    
    DefaultPawnClass = nullptr;
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (auto* GI = Cast<UMyGameInstance>(GetGameInstance()))
    {

        GI->ResetLoginBGM();
    }
}

