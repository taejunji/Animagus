// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundResultWidget.h"
#include "RoundResultRowWidget.h"
#include "Components/VerticalBox.h"
#include "TimerManager.h"

void URoundResultWidget::SetupResults(
    const TArray<FString>& IDs,
    const TArray<int32>& Scores,
    float DisplayTime)
{
    if (!ResultList || !RowWidgetClass) return;

    ResultList->ClearChildren();
    int32 Count = FMath::Min(IDs.Num(), Scores.Num());
    for (int32 i = 0; i < Count; ++i)
    {
        
        URoundResultRowWidget* Row = CreateWidget<URoundResultRowWidget>(this, RowWidgetClass);
        if (Row)
        {
            Row->SetupRow(IDs[i], Scores[i]);
            ResultList->AddChild(Row);
        }
    }

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            HideTimerHandle,
            this, &URoundResultWidget::HideSelf,
            DisplayTime, false
        );
    }
}

void URoundResultWidget::SetupResults(const TArray<FString>& IDs, const TArray<int32>& Scores)
{
    if (!ResultList || !RowWidgetClass) return;

    ResultList->ClearChildren();
    int32 Count = FMath::Min(IDs.Num(), Scores.Num());
    for (int32 i = 0; i < Count; ++i)
    {
        
        URoundResultRowWidget* Row = CreateWidget<URoundResultRowWidget>(this, RowWidgetClass);
        if (Row)
        {
            Row->SetupRow(IDs[i], Scores[i]);
            ResultList->AddChild(Row);
        }
    }
}

void URoundResultWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void URoundResultWidget::HideSelf()
{
    RemoveFromParent();
}
