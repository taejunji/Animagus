// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "../PlayerController/MainMenuController.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::PlayHoverSound()
{
    if (HoverSound)
        UGameplayStatics::PlaySound2D(this, HoverSound); 
}

void UMainMenuWidget::PlayClickSound()
{
    if (ClickSound)
        UGameplayStatics::PlaySound2D(this, ClickSound); 
}

void UMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void UMainMenuWidget::BindButtons()
{
    if (!Owner.IsValid()) return;

    if (BtnStart)
    {
        BtnStart->OnClicked.AddDynamic(Owner.Get(), &AMainMenuController::OnStartClicked);
        BtnStart->OnClicked.AddDynamic(this, &UMainMenuWidget::PlayClickSound);
        BtnStart->OnHovered.AddDynamic(this, &UMainMenuWidget::PlayHoverSound); 
    }
    if (BtnSettings){
        BtnSettings->OnClicked.AddDynamic(Owner.Get(), &AMainMenuController::OnSettingsClicked);
        BtnSettings->OnClicked.AddDynamic(this, &UMainMenuWidget::PlayClickSound);
        BtnSettings->OnHovered.AddDynamic(this, &UMainMenuWidget::PlayHoverSound); 
    }
}

