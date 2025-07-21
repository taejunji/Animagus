// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsWidget.h"
#include "Components/Slider.h"
#include "Components/ComboBoxString.h"
#include "../PlayerController/MainMenuController.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void USettingsWidget::SetupOwner(AMainMenuController* InOwner)
{
    UE_LOG(LogTemp, Warning, TEXT("SettingsWidget::SetupOwner()"));
    Owner = InOwner;
    BindControls();
}

void USettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UpdateImageSelectionVisuals();
}

void USettingsWidget::OnSensitivityChanged(float NewValue)
{
    if (Owner == nullptr) return;
        Owner->HandleSensitivityChanged(NewValue);
}

void USettingsWidget::BindControls()
{
    UE_LOG(LogTemp, Warning, TEXT("SettingsWidget::BindControls()"));
    if (Owner == nullptr) return;
    
    if (VolumeSlider)
    {
        VolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::OnVolumeChanged);
        VolumeSlider->OnMouseCaptureEnd.AddDynamic(this, &USettingsWidget::PlaySliderSound);
        UE_LOG(LogTemp, Log, TEXT("  Bound VolumeSlider"));
    }
    if (QualityCombo)
    {
       
        QualityCombo->ClearOptions();
        QualityCombo->AddOption(TEXT("Low"));
        QualityCombo->AddOption(TEXT("Medium"));
        QualityCombo->AddOption(TEXT("High"));
        QualityCombo->AddOption(TEXT("Epic"));

        QualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::OnQualityChanged);
        UE_LOG(LogTemp, Log, TEXT("  Bound QualityCombo"));
    }

    if (SensitivitySlider)
    {
        SensitivitySlider->OnValueChanged.AddDynamic(
                 this,
                 &USettingsWidget::OnSensitivityChanged  // 이 함수는 SettingsWidget 내에서
             );
        SensitivitySlider->OnMouseCaptureEnd.AddDynamic(this, &USettingsWidget::PlaySliderSound);
        UE_LOG(LogTemp, Log, TEXT("  Bound SensitivitySlider"));
    }

    
    if (BtnConfirm)
    {
        BtnConfirm->OnClicked.AddDynamic(this, &USettingsWidget::OnConfirmClicked);
        BtnConfirm->OnHovered.AddDynamic(this, &USettingsWidget::PlayHoverSound); 
    }
    if (BtnImg0)
    {
        BtnImg0->OnClicked.AddDynamic(this, &USettingsWidget::OnSelectImg0);
        BtnImg0->OnHovered.AddDynamic(this, &USettingsWidget::PlayHoverSound); 
    }
    if (BtnImg1)
    {
        BtnImg1->OnClicked.AddDynamic(this, &USettingsWidget::OnSelectImg1);
        BtnImg1->OnHovered.AddDynamic(this, &USettingsWidget::PlayHoverSound); 
    }
    if (BtnImg2)
    {
        BtnImg2->OnClicked.AddDynamic(this, &USettingsWidget::OnSelectImg2);
        BtnImg2->OnHovered.AddDynamic(this, &USettingsWidget::PlayHoverSound); 
    }
    if (BtnImg3)
    {
        BtnImg3->OnClicked.AddDynamic(this, &USettingsWidget::OnSelectImg3);
        BtnImg3->OnHovered.AddDynamic(this, &USettingsWidget::PlayHoverSound);  
    }
}
void USettingsWidget::OnVolumeChanged(float NewValue)
{
    if (Owner == nullptr) return;
        Owner->HandleVolumeChanged(NewValue);
}

void USettingsWidget::OnQualityChanged(FString SelectedItem, ESelectInfo::Type SelectType)
{
    if (Owner == nullptr) return;
        Owner->HandleQualityChanged(SelectedItem);
}

void USettingsWidget::OnConfirmClicked()
{
    // SettingsWidget 닫기
    PlayClickSound();
    RemoveFromParent();
    
}

void USettingsWidget::OnSelectImg0()
{
    PlayClickSound();
    SelectedImageIndex = 0;
    Owner->HandleHUDImageIndexChanged(0);
    UpdateImageSelectionVisuals();
}
void USettingsWidget::OnSelectImg1()
{
    PlayClickSound();
    SelectedImageIndex = 1;
    Owner->HandleHUDImageIndexChanged(1);
    UpdateImageSelectionVisuals();
}
void USettingsWidget::OnSelectImg2()
{
    PlayClickSound();
    SelectedImageIndex = 2;
    Owner->HandleHUDImageIndexChanged(2);
    UpdateImageSelectionVisuals();
}

void USettingsWidget::OnSelectImg3()
{
    PlayClickSound();
    SelectedImageIndex = 3;
    Owner->HandleHUDImageIndexChanged(3);
    UpdateImageSelectionVisuals();
}

void USettingsWidget::UpdateImageSelectionVisuals()
{
    // 기본 테두리 색
    const FLinearColor UnselectedColor = FLinearColor(0,0,0,0.4);

    // 선택 시 강조 색상
    const FLinearColor SelectedColor   = FLinearColor(0.2f, 0.6f, 1.f, 0.5f);

    if (BorderImg0) BorderImg0->SetBrushColor( SelectedImageIndex==0 ? SelectedColor : UnselectedColor );
    if (BorderImg1) BorderImg1->SetBrushColor( SelectedImageIndex==1 ? SelectedColor : UnselectedColor );
    if (BorderImg2) BorderImg2->SetBrushColor( SelectedImageIndex==2 ? SelectedColor : UnselectedColor );
    if (BorderImg2) BorderImg3->SetBrushColor( SelectedImageIndex==3 ? SelectedColor : UnselectedColor );
}

void USettingsWidget::PlaySliderSound()
{
    if (ClickSound)
        UGameplayStatics::PlaySound2D(this, ClickSound);
}



void USettingsWidget::PlayHoverSound()
{
    if (HoverSound)
        UGameplayStatics::PlaySound2D(this, HoverSound); 
}

void USettingsWidget::PlayClickSound()
{
    if (ClickSound)
        UGameplayStatics::PlaySound2D(this, ClickSound); 
}

