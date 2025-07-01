// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "../PlayerController/MainMenuController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
    
    PlayerControllerClass = AMainMenuController::StaticClass();
    
    DefaultPawnClass = nullptr;
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
   
}

