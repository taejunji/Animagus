// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundResultRowWidget.h"
#include "Components/TextBlock.h"

void URoundResultRowWidget::SetupRow(const FString& ID, int32 Score)
{
    if (TxtID)    TxtID->SetText(FText::FromString(ID));
    if (TxtScore) TxtScore->SetText(FText::AsNumber(Score));
}
