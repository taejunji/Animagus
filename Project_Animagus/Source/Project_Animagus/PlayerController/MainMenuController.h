// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

class UMainMenuWidget;
class USettingsWidget;
class USoundMix;
class USoundClass;

UCLASS()
class PROJECT_ANIMAGUS_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    /** Start 버튼 클릭 */
    UFUNCTION() void OnStartClicked();

    /** Settings 버튼 클릭 */
    UFUNCTION() void OnSettingsClicked();


    void HandleVolumeChanged(float NewVolume);
    void HandleQualityChanged(const FString& SelectedItem);
    void HandleSensitivityChanged(float NewSensitivity);

    UFUNCTION()
    void HandleHUDImageIndexChanged(int32 NewIndex);

protected:
    // 본체위젯
    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    // 본체 인스턴스
    UPROPERTY()
    UMainMenuWidget* MainMenuWidget;

    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<USettingsWidget> SettingsWidgetClass;

    UPROPERTY()
    USettingsWidget* SettingsWidget;
    
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundMix* MasterVolumeMix;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundClass* MasterSoundClass;

    bool bMixPushed = false;
    
    // 마우스 감도
    float CachedMouseSensitivity = 1.0f;
};
