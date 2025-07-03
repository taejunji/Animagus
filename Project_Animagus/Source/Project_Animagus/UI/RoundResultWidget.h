// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundResultWidget.generated.h"

/**
 * 
 */
class URoundResultRowWidget;
class UVerticalBox;

UCLASS()
class PROJECT_ANIMAGUS_API URoundResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    
    UFUNCTION()
    void SetupResults(
        const TArray<FString>& IDs,
        const TArray<int32>& Scores,
        float DisplayTime
    );

    void SetupResults(
        const TArray<FString>& IDs,
        const TArray<int32>& Scores
    );
    
protected:
    virtual void NativeConstruct() override;

    //
    UPROPERTY(meta=(BindWidget))
    UVerticalBox* ResultList;

    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<URoundResultRowWidget> RowWidgetClass;
    
private:
    FTimerHandle HideTimerHandle;

    /** 화면에서 제거 */
    void HideSelf();
};
