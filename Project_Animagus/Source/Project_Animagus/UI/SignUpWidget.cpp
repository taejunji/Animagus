// Fill out your copyright notice in the Description page of Project Settings.


#include "SignUpWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/PlayerController/LoginPlayerController.h"


void USignUpWidget::SetupOwner(ALoginPlayerController* InOwner)
{
    Owner = TWeakObjectPtr<ALoginPlayerController>(InOwner);
}

void USignUpWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (TxtResult)
        TxtResult->SetVisibility(ESlateVisibility::Collapsed);
}

void USignUpWidget::ShowResult(const FText& Message)
{
    if (!TxtResult) return;
    TxtResult->SetText(Message);
    TxtResult->SetVisibility(ESlateVisibility::Visible);

    //FTimerHandle UnusedHandle;
    //GetWorldTimerManager().SetTimer(
    //    UnusedHandle,
    //    [this]()
    //    {
    //        TxtResult->SetVisibility(ESlateVisibility::Collapsed);
    //    },
    //    2.0f,
    //    false  // 한번만
    //);
}

void USignUpWidget::PlayHoverSound()
{
    if (HoverSound)
        UGameplayStatics::PlaySound2D(this, HoverSound);
}

void USignUpWidget::PlayClickSound()
{
    if (ClickSound)
        UGameplayStatics::PlaySound2D(this, ClickSound);
}

void USignUpWidget::BindButtons()
{
    if (!Owner.IsValid())
        return;

    if (BtnOk)
    {
        BtnOk->OnClicked.AddDynamic(Owner.Get(), &ALoginPlayerController::OnSignOkClicked);
        BtnOk->OnClicked.AddDynamic(this, &USignUpWidget::PlayClickSound);
        BtnOk->OnHovered.AddDynamic(this, &USignUpWidget::PlayHoverSound);
    }
    if (BtnCancel)
    {
        BtnCancel->OnClicked.AddDynamic(Owner.Get(), &ALoginPlayerController::OnSignCancelClicked);
        BtnCancel->OnClicked.AddDynamic(this, &USignUpWidget::PlayClickSound);
        BtnCancel->OnHovered.AddDynamic(this, &USignUpWidget::PlayHoverSound);
    }
}

void USignUpWidget::ClearInputs()
{
    if (TxtUserID)   TxtUserID->SetText(FText::GetEmpty());
    if (TxtPassword) TxtPassword->SetText(FText::GetEmpty());
    if (TxtNickName) TxtNickName->SetText(FText::GetEmpty());
}
