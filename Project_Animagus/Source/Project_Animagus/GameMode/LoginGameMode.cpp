// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "../System//MyGameInstance.h"

ALoginGameMode::ALoginGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

}

void ALoginGameMode::BeginPlay()
{
    Super::BeginPlay();
    
}

void ALoginGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Cast<UMyGameInstance>(GWorld->GetGameInstance())->HandleRecvPackets();
}
