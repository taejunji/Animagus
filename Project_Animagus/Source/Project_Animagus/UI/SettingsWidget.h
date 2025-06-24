// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsWidget.generated.h"


class USlider;
class UComboBoxString;
class AMainMenuController;
class UButton;
class UBorder;
/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    /** 컨트롤러 참조 바인딩 */
    UFUNCTION()
    void SetupOwner(AMainMenuController* InOwner); 
    
    virtual void NativeConstruct() override;
    
    UPROPERTY(meta=(BindWidget)) USlider* VolumeSlider;
    
    UPROPERTY(meta=(BindWidget)) UComboBoxString* QualityCombo;

    UPROPERTY(meta=(BindWidget)) USlider* SensitivitySlider;

    // 1) Confirm 버튼
    UPROPERTY(meta=(BindWidget)) UButton* BtnConfirm;

    UPROPERTY(meta=(BindWidget))
    UBorder* BorderImg0;

    UPROPERTY(meta=(BindWidget))
    UBorder* BorderImg1;

    UPROPERTY(meta=(BindWidget))
    UBorder* BorderImg2;

    UPROPERTY(meta=(BindWidget))
    UBorder* BorderImg3;

    UPROPERTY(meta=(BindWidget))
    UButton* BtnImg0;

    UPROPERTY(meta=(BindWidget))
    UButton* BtnImg1;

    UPROPERTY(meta=(BindWidget))
    UButton* BtnImg2;

    UPROPERTY(meta=(BindWidget))
    UButton* BtnImg3;
    
    AMainMenuController* Owner = nullptr;

    UFUNCTION()
    void OnSensitivityChanged(float NewValue);

    void BindControls();
    
    UFUNCTION()
    void OnVolumeChanged(float NewValue);
    
    UFUNCTION()
    void OnQualityChanged(FString SelectedItem, ESelectInfo::Type SelectType);

    int32 SelectedImageIndex = -1;
    
    UFUNCTION()
    void OnConfirmClicked();

    UFUNCTION()
    void OnSelectImg0();

    UFUNCTION()
    void OnSelectImg1();

    UFUNCTION()
    void OnSelectImg2();

    UFUNCTION()
    void OnSelectImg3();
    
    void UpdateImageSelectionVisuals();

    UFUNCTION()
    void PlaySliderSound();

 
public:
    
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* ClickSound;


    
    UFUNCTION()
    void PlayHoverSound();

    UFUNCTION()
    void PlayClickSound(); 
};
