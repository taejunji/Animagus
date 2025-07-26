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
        BtnQuit->OnHovered.AddDynamic(this, &UResultWidget::PlayHoverSound);
    }
}

void UResultWidget::ShowOutcome(bool bIsWinner)
{
    if (!TxtOutcome) return;
    TxtOutcome->SetText(
        FText::FromString(bIsWinner ? TEXT("우승!") : TEXT("패배.."))
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
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("QuitGame 실패: PlayerController가 없습니다."));
        return;
    }

    // 2) QuitGame 호출
    UKismetSystemLibrary::QuitGame(
        this,           // WorldContextObject: UUserWidget은 OK
        PC,             // SpecificPlayer: nullptr 금지!
        EQuitPreference::Quit,
        true            // bIgnorePlatformRestrictions
    );
}
