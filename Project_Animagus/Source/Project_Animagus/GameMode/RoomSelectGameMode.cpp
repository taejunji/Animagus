// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomSelectGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "../System//MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerController/RoomSelectController.h"
#include "../Server/Server/protocol.h"


ARoomSelectGameMode::ARoomSelectGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    PlayerControllerClass = ARoomSelectController::StaticClass();
    
    DefaultPawnClass = nullptr;
}

void ARoomSelectGameMode::BeginPlay()
{
    Super::BeginPlay();
    
}

void ARoomSelectGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Cast<UMyGameInstance>(GWorld->GetGameInstance())->HandleRecvPackets();
}

void ARoomSelectGameMode::HandleRoomEnter()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (nullptr == PC) return;
    if (ARoomSelectController* RoomPC = Cast<ARoomSelectController>(PC))
    {
        RoomPC->HandleRoomEnter();
    }

}

void ARoomSelectGameMode::HandleRoomEnterFail()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (nullptr == PC) return;
    if (ARoomSelectController* RoomPC = Cast<ARoomSelectController>(PC))
    {
        RoomPC->HandleRoomEnterFail();
    }
}

