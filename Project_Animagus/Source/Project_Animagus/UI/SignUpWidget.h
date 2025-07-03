// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SignUpWidget.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;
class ALoginPlayerController;
class USoundBase;

UCLASS()
class PROJECT_ANIMAGUS_API USignUpWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

    UFUNCTION()
    void SetupOwner(ALoginPlayerController* InOwner);

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget)) UEditableTextBox* TxtUserID;
    UPROPERTY(meta = (BindWidget)) UEditableTextBox* TxtPassword;
    UPROPERTY(meta = (BindWidget)) UEditableTextBox* TxtNickName;

    UPROPERTY(meta = (BindWidget)) UButton* BtnOk;
    UPROPERTY(meta = (BindWidget)) UButton* BtnCancel;

    UPROPERTY(meta = (BindWidget)) UTextBlock* TxtResult;

    UFUNCTION()
    void ShowResult(const FText& Message);

    TWeakObjectPtr<ALoginPlayerController> Owner;

    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundBase* ClickSound;

    UFUNCTION()
    void PlayHoverSound();

    UFUNCTION()
    void PlayClickSound();

    void BindButtons();

    UFUNCTION()
    void ClearInputs();
};
