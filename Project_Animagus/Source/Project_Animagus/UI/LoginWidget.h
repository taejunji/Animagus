// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;
class ALoginPlayerController;
class USoundBase;

UCLASS()
class PROJECT_ANIMAGUS_API ULoginWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    
    UFUNCTION()
    void SetupOwner(ALoginPlayerController* InOwner) { Owner = InOwner; }


    virtual void NativeConstruct() override;

    UFUNCTION()
    void ShowResult(const FText& Message);
    
    UFUNCTION()
    void HideResult();
    
    UPROPERTY(meta=(BindWidget)) UEditableTextBox* TxtUserID;
    UPROPERTY(meta=(BindWidget)) UEditableTextBox* TxtPassword;

    
    UPROPERTY(meta=(BindWidget)) UButton* BtnLogin;
    UPROPERTY(meta=(BindWidget)) UButton* BtnSignup;

    
    UPROPERTY(meta=(BindWidget)) UTextBlock* TxtResult;
    
    TWeakObjectPtr<ALoginPlayerController> Owner;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* ClickSound;

    UFUNCTION()
    void PlayHoverSound();

    UFUNCTION()
    void PlayClickSound();

    void BindButtons();
	
};
