// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuController.h"

#include "AudioDevice.h"
#include "../UI/MainMenuWidget.h"
#include "Components/Slider.h"
#include "../UI/SettingsWidget.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/System/MyGameInstance.h"
#include "Project_Animagus/UI/SettingsWidget.h"


void AMainMenuController::BeginPlay()
{
    Super::BeginPlay();

    if (!MainMenuWidgetClass) return;

    MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuWidgetClass);
    if (!MainMenuWidget) return;

    MainMenuWidget->AddToViewport();
    MainMenuWidget->SetupOwner(this);
    MainMenuWidget->SetIsFocusable(true);

    // UI 전용 입력 모드
    FInputModeUIOnly Mode;
    Mode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
    bShowMouseCursor = true;

    GetMousePosition(LastMousPosition.X, LastMousPosition.Y);
    bCursorHidden = false;
    InactivityTime = 0.f;

    PrimaryActorTick.bCanEverTick = true;
}

void AMainMenuController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector2D CurrentMousePosition;
    if (GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y))
    {
        if (false == CurrentMousePosition.Equals(LastMousPosition, 1.0f))
        {
            // 마우스 움직였음
            LastMousPosition = CurrentMousePosition;
            InactivityTime = 0.f;

            if (bCursorHidden)
            {
                bShowMouseCursor = true;
                bCursorHidden = false;

                // 커서 표시할 땐 다시 UIOnly 모드로 설정
                FInputModeUIOnly InputMode;
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
                SetInputMode(InputMode);
            }
        }
        else
        {
            // 마우스 움직임 없음 
            InactivityTime += DeltaTime;
            if (false == bCursorHidden && InactivityTime >= CursorHideDelay)
            {
                SetHideCursor();
            }
        }
    }
    else
    {
        // 마우스 위치 자체를 가져올 수 없을 경우에도 시간 증가
        InactivityTime += DeltaTime;

        if (!bCursorHidden && InactivityTime >= CursorHideDelay)
        {
            SetHideCursor();
        }
    }
}

void AMainMenuController::OnStartClicked()
{
    UGameplayStatics::OpenLevel(this, TEXT("L_RoomSelect"));
}

void AMainMenuController::OnSettingsClicked()
{
    if (SettingsWidget && SettingsWidget->IsInViewport())
    {
        return;
    }
    
    if (!SettingsWidgetClass) return;
    SettingsWidget = CreateWidget<USettingsWidget>(this, SettingsWidgetClass);
    SettingsWidget->AddToViewport();
    SettingsWidget->SetupOwner(this);

    FInputModeGameAndUI Mode;
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
    bShowMouseCursor = true;
    SettingsWidget->AddToViewport(100);
    
    // 마우스 감도 초기값을 슬라이더에 세팅
    if (SettingsWidget->VolumeSlider)
    {
        float SavedVol = 1.0f;
        if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
        {
            SavedVol = GI->MasterVolume;
        }
        SettingsWidget->VolumeSlider->SetValue(SavedVol);
    }

    if (SettingsWidget->SensitivitySlider)
    {
        if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
        {
            SettingsWidget->SensitivitySlider->SetValue(GI->MouseSensitivity);
        }
    }
}

// 사운드 볼륨 조절
void AMainMenuController::HandleVolumeChanged(float NewVolume)
{
    if (MasterVolumeMix && MasterSoundClass)
    {

        if (!bMixPushed)
        {
            UGameplayStatics::PushSoundMixModifier(this, MasterVolumeMix);
            bMixPushed = true;
        }

        UGameplayStatics::SetSoundMixClassOverride(
            this,
            MasterVolumeMix,
            MasterSoundClass,
            NewVolume,    // Volume
            1.0f,
            0.0f,         // FadeOutTime
            true          // bApplyToChildren
        );

        // ▶ 변경된 볼륨을 GameInstance에 저장
        if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
        {
            GI->MasterVolume = NewVolume;
        }
    }
}

// 그래픽 퀄리티 조절 (0=Low,1=Medium,2=High,3=Epic)
void AMainMenuController::HandleQualityChanged(const FString& SelectedItem)
{
    UE_LOG(LogTemp, Log, TEXT("HandleQualityChanged: %s"), *SelectedItem);
    
    int32 Level = 2;  // default High
    if (SelectedItem == TEXT("Low"))    Level = 0;
    else if (SelectedItem == TEXT("Medium")) Level = 1;
    else if (SelectedItem == TEXT("High"))   Level = 2;
    else if (SelectedItem == TEXT("Epic"))   Level = 3;

    if (GEngine)
    {
        if (UGameUserSettings* EngineSet = GEngine->GetGameUserSettings())
        {
            EngineSet->SetOverallScalabilityLevel(Level);
            EngineSet->ApplySettings(true);
            EngineSet->SaveSettings();
        }
    }
}

// 마우스 감도 조절
void AMainMenuController::HandleSensitivityChanged(float NewSensitivity)
{
    UE_LOG(LogTemp, Log, TEXT("HandleSensitivityChanged: %f"), NewSensitivity);
    if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
    {
        GI->MouseSensitivity = NewSensitivity;
    }
   
}

void AMainMenuController::HandleHUDImageIndexChanged(int32 NewIndex)
{
    if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
    {
        GI->AimImageIndex = NewIndex;
        UE_LOG(LogTemp, Log, TEXT("HUDImageIndex set to %d"), NewIndex);
    }
}

void AMainMenuController::SetHideCursor()
{
    bShowMouseCursor = false;
    bCursorHidden = true;

    // 마우스 커서를 숨기려면 GameOnly 모드로 전환
    FInputModeGameOnly GameOnlyInput;
    SetInputMode(GameOnlyInput);
}
