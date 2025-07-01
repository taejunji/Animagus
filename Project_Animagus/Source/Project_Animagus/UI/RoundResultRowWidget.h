// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundResultRowWidget.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class PROJECT_ANIMAGUS_API URoundResultRowWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    UFUNCTION()
    void SetupRow(const FString& ID, int32 Score);

protected:
    UPROPERTY(meta=(BindWidget)) UTextBlock* TxtID;
    UPROPERTY(meta=(BindWidget)) UTextBlock* TxtScore;	
};
