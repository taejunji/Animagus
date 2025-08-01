// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomSelectWidget.generated.h"

class UButton;
class UTextBlock;
class ARoomSelectController;

UCLASS()
class PROJECT_ANIMAGUS_API URoomSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    
    
    UFUNCTION()
    void SetupOwner(ARoomSelectController* InOwner); 

    UFUNCTION()
    void ShowResult(const FText& Message);
    FText InitMessage;

protected:
    virtual void NativeConstruct() override;

    void BindAllButtons();

    UPROPERTY(meta=(BindWidget)) UButton* BtnRoom1;
    UPROPERTY(meta=(BindWidget)) UButton* BtnRoom2;
    UPROPERTY(meta=(BindWidget)) UButton* BtnRoom3;
    UPROPERTY(meta=(BindWidget)) UButton* BtnRoom4;
    UPROPERTY(meta=(BindWidget)) UButton* BtnRoom5;

    UPROPERTY(meta=(BindWidget)) UTextBlock* DragonText;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* ClickSound;
    
    TWeakObjectPtr<ARoomSelectController> Owner;

};
