// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "../PlayerController/MainMenuController.h"

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void UMainMenuWidget::BindButtons()
{
    if (!Owner.IsValid()) return;

    if (BtnStart)
        BtnStart->OnClicked.AddDynamic(Owner.Get(), &AMainMenuController::OnStartClicked);

    if (BtnSettings)
        BtnSettings->OnClicked.AddDynamic(Owner.Get(), &AMainMenuController::OnSettingsClicked);
}

