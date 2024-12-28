// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"

void ABattleGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Display, TEXT("Battle Game Mode Begin"));
}

ABattleGameMode::ABattleGameMode()
{
    
}