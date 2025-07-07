// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultGameMode.h"
#include "../PlayerController/ResultPlayerController.h"


void AResultGameMode::BeginPlay()
{
    Super::BeginPlay();
    
}

AResultGameMode::AResultGameMode()
{
    DefaultPawnClass = nullptr;
}