// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuController.h"
#include "../UI/MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuController::BeginPlay()
{
    Super::BeginPlay();

    if (!MainMenuWidgetClass) return;

    MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);
    if (!MainMenuWidget) return;

    MainMenuWidget->AddToViewport();
    MainMenuWidget->SetupOwner(this);

    // UI 전용 입력 모드
    FInputModeUIOnly Mode;
    Mode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
    bShowMouseCursor = true;
}

void AMainMenuController::OnStartClicked()
{
    // 게임 플레이 레벨 이름을 "GameMap"으로 가정
    UGameplayStatics::OpenLevel(this, TEXT("L_RoomSelect"));
}

void AMainMenuController::OnSettingsClicked()
{
    // 
}