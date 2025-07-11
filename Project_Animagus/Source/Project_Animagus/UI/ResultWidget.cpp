// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UResultWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BtnConfirm){
        BtnConfirm->OnClicked.AddDynamic(this, &UResultWidget::OnConfirmClicked);
        BtnConfirm->OnHovered.AddDynamic(this, &UResultWidget::PlayHoverSound); 
        
    }

    if (BtnQuit){
        BtnQuit->OnClicked.AddDynamic(this, &UResultWidget::OnQuitClicked);
        BtnConfirm->OnHovered.AddDynamic(this, &UResultWidget::PlayHoverSound); 
    }
}

void UResultWidget::ShowOutcome(bool bIsWinner)
{
    if (!TxtOutcome) return;
    TxtOutcome->SetText(
        FText::FromString(bIsWinner ? TEXT("Win") : TEXT("Lose"))
    );
}

void UResultWidget::OnConfirmClicked()
{

    if (ClickSound)
        UGameplayStatics::PlaySound2D(this, ClickSound);
    
    UGameplayStatics::OpenLevel(this, TEXT("L_MainMenu"));
}

void UResultWidget::PlayHoverSound()
{
    if (HoverSound)
        UGameplayStatics::PlaySound2D(this, HoverSound); 
}

void UResultWidget::OnQuitClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true); 
}
