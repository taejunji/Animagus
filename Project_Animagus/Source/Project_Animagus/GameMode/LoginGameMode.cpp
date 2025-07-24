// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Project_Animagus/System/MyGameInstance.h"

ALoginGameMode::ALoginGameMode()
{
 
}

void ALoginGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (auto* GI = Cast<UMyGameInstance>(GetGameInstance()))
    {
        GI->ResetMenuBGM();
    }
}