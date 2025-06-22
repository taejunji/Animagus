// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/PlayerController/LoginPlayerController.h"

void ULoginWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 처음엔 결과 메시지 숨기기
    if (TxtResult)
        TxtResult->SetVisibility(ESlateVisibility::Collapsed);

}

void ULoginWidget::ShowResult(const FText& Message)
{
    if (!TxtResult) return;
    TxtResult->SetText(Message);
    TxtResult->SetVisibility(ESlateVisibility::Visible);
}

void ULoginWidget::HideResult()
{
    if (!TxtResult) return;
    TxtResult->SetVisibility(ESlateVisibility::Collapsed);
}

void ULoginWidget::PlayHoverSound()
{
    if (HoverSound)
        UGameplayStatics::PlaySound2D(this, HoverSound); 
}

void ULoginWidget::PlayClickSound()
{
    if (ClickSound)
        UGameplayStatics::PlaySound2D(this, ClickSound); 
}

void ULoginWidget::BindButtons()
{
    if (!Owner.IsValid())
        return;

    if (BtnLogin)
    {
        BtnLogin->OnClicked.AddDynamic(Owner.Get(), &ALoginPlayerController::OnLoginClicked);
        BtnLogin->OnClicked.AddDynamic(this, &ULoginWidget::PlayClickSound); 
        BtnLogin->OnHovered.AddDynamic(this, &ULoginWidget::PlayHoverSound); 
    }
    if (BtnSignup)
    {
        BtnSignup->OnClicked.AddDynamic(Owner.Get(), &ALoginPlayerController::OnSignupClicked);
        BtnSignup->OnClicked.AddDynamic(this, &ULoginWidget::PlayClickSound);
        BtnSignup->OnHovered.AddDynamic(this, &ULoginWidget::PlayHoverSound); 
    }
}
