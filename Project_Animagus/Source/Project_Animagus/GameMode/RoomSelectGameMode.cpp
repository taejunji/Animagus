// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomSelectGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "../PlayerController/RoomSelectController.h"


ARoomSelectGameMode::ARoomSelectGameMode()
{
   
    PlayerControllerClass = ARoomSelectController::StaticClass();
    
    DefaultPawnClass = nullptr;
}

void ARoomSelectGameMode::BeginPlay()
{
    Super::BeginPlay();
    
}

