// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"



class UButton;
class AMainMenuController;

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

private:
    TWeakObjectPtr<AMainMenuController> Owner;
    
    void BindButtons();
};
