// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h" // 나중에 스킬 목록 추가 시 사용

void UMyPlayerHUDWidget::UpdateHP(float HPPercent)
{
    if (HPProgressBar)
    {
        HPProgressBar->SetPercent(HPPercent);
    }
}
