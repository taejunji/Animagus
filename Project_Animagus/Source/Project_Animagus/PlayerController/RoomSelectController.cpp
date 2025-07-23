// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomSelectController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Project_Animagus/GameMode/BattleGameMode.h"
#include "Project_Animagus/UI/RoomSelectWidget.h"


void ARoomSelectController::BeginPlay()
{
    Super::BeginPlay();
    ShowRoomSelectUI();

}

void ARoomSelectController::ShowRoomSelectUI()
{
    if (!RoomSelectWidgetClass) return;

    RoomSelectWidget = CreateWidget<URoomSelectWidget>(this, RoomSelectWidgetClass);
    if (!RoomSelectWidget) return;

    RoomSelectWidget->AddToViewport();
    RoomSelectWidget->SetupOwner(this);
    RoomSelectWidget->SetIsFocusable(true);

    // 입력 모드 전환(선택 UI 전용)
    FInputModeUIOnly Mode;
    Mode.SetWidgetToFocus(RoomSelectWidget->TakeWidget());
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
    bShowMouseCursor = true;
}

void ARoomSelectController::EnterRoom()
{
    // 서버상 룸 입장
}

void ARoomSelectController::PlayHoverSound()
{
    if (HoverSound)
        UGameplayStatics::PlaySound2D(this, HoverSound);
}

void ARoomSelectController::PlayClickSound()
{
    if (ClickSound)
        UGameplayStatics::PlaySound2D(this, ClickSound);
}

void ARoomSelectController::OnRoom1Clicked()
{
    PlayClickSound();

    EnterRoom();
    
    UGameplayStatics::OpenLevel(this, TEXT("L_Connect"));
}

void ARoomSelectController::OnRoom2Clicked()
{
    PlayClickSound();

    EnterRoom();
    
    UGameplayStatics::OpenLevel(this, TEXT("L_Connect"));
}

void ARoomSelectController::OnRoom3Clicked()
{
    PlayClickSound();
    
    EnterRoom();
    
    UGameplayStatics::OpenLevel(this, TEXT("L_Connect"));
}

void ARoomSelectController::OnRoom4Clicked()
{
    PlayClickSound();

    EnterRoom();
    
    UGameplayStatics::OpenLevel(this, TEXT("L_Connect"));
}

void ARoomSelectController::OnRoom5Clicked()
{
    PlayClickSound();

    EnterRoom();
    
    UGameplayStatics::OpenLevel(this, TEXT("L_Connect"));
}