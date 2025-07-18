// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"



class UButton;
class AMainMenuController;
class UImage;
class UMediaSource;
class UMediaPlayer;
class UMediaTexture; 
/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
    
public:
    
    UFUNCTION()
    void SetupOwner(AMainMenuController* InOwner) { Owner = InOwner; BindButtons(); }

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* ClickSound;

    UFUNCTION()
    void PlayHoverSound();

    UFUNCTION()
    void PlayClickSound();
    
protected:
    virtual void NativeConstruct() override;
    
    UPROPERTY(meta=(BindWidget))
    UButton* BtnStart;

    UPROPERTY(meta=(BindWidget))
    UButton* BtnSettings;

    UPROPERTY(meta=(BindWidget))
    UButton* BtnQuit;

    UPROPERTY(meta = (BindWidget))
    UImage* VideoImage;

    /** 에디터에서 할당할 MediaPlayer */
    UPROPERTY(EditAnywhere, Category="Media")
    UMediaPlayer* MediaPlayer;

    /** MediaPlayer가 출력하는 텍스처 */
    UPROPERTY(EditAnywhere, Category="Media")
    UMediaTexture* MediaTexture;

    /** 재생할 미디어 소스 (FileMediaSource) */
    UPROPERTY(EditAnywhere, Category="Media")
    UMediaSource* MediaSource;
    
private:
    TWeakObjectPtr<AMainMenuController> Owner;
    
    void BindButtons();

    UFUNCTION()
    void OnQuitClicked();
};
