// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "../PlayerController/MainMenuController.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/MediaAssets/Public/MediaPlayer.h"
#include "Runtime/MediaAssets/Public/MediaTexture.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MediaSource.h"

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

    if (MediaPlayer && MediaSource && MediaTexture && VideoImage)
    {
        // 1) MediaPlayer에 소스 열기
        MediaPlayer->OpenSource(MediaSource);

        // 2) MediaTexture를 MediaPlayer에 바인딩
        MediaTexture->SetMediaPlayer(MediaPlayer);

        // 3) UMG Image에 브러시 텍스처로 설정
        FSlateBrush Brush;
        Brush.SetResourceObject(MediaTexture);
        VideoImage->SetBrush(Brush);

        // 4) 자동 재생
        MediaPlayer->Play();
    }
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
    if (BtnQuit)
    {
        BtnQuit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
        BtnQuit->OnHovered.AddDynamic(this, &UMainMenuWidget::PlayHoverSound); 
    }
}

void UMainMenuWidget::OnQuitClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

