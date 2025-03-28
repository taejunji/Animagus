// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h" // 나중에 스킬 목록 추가 시 사용
#include "Components/TextBlock.h"

void UMyPlayerHUDWidget::UpdateHP(float HPPercent)
{
    if (HPProgressBar)
    {
        HPProgressBar->SetPercent(HPPercent);
    }
}

void UMyPlayerHUDWidget::UpdateSkillIcon(int32 SkillIndex, UTexture2D* NewTexture)
{
    UImage* TargetImage = nullptr;
    switch(SkillIndex)
    {
    case 0: TargetImage = SkillImage0; break;
    case 1: TargetImage = SkillImage1; break;
    case 2: TargetImage = SkillImage2; break;
    case 3: TargetImage = SkillImage3; break;
    default: break;
    }
    
    if (TargetImage && NewTexture)
    {
        FSlateBrush NewBrush;
        NewBrush.SetResourceObject(NewTexture);
        // 필요시 ImageSize 조절
        NewBrush.ImageSize = FVector2D(64.f, 64.f);
        TargetImage->SetBrush(NewBrush);
    }
}

void UMyPlayerHUDWidget::UpdateSkillCooldown(int32 SkillIndex, float CooldownPercent)
{
    UProgressBar* TargetBar = nullptr;
    switch(SkillIndex)
    {
    case 0: TargetBar = SkillCooldownProgressBar0; break;
    case 1: TargetBar = SkillCooldownProgressBar1; break;
    case 2: TargetBar = SkillCooldownProgressBar2; break;
    case 3: TargetBar = SkillCooldownProgressBar3; break;
    default: break;
    }

    if (TargetBar)
    {
        TargetBar->SetPercent(CooldownPercent);
    }
}

void UMyPlayerHUDWidget::UpdateCountdown(float CountdownValue)
{
    if (CountdownText)
    {
        // CountdownValue가 1초 이상이면 (즉, ceil값이 1보다 크면) 표시하고, 
        // 그렇지 않으면(1초 이하) 빈 텍스트로 처리.
        float DisplayTime = FMath::CeilToFloat(CountdownValue);
        if (DisplayTime > 0)
        {
            FString NewText = FString::Printf(TEXT("%.0f"), DisplayTime);
            CountdownText->SetText(FText::FromString(NewText));
        }
        else
        {
            CountdownText->SetText(FText::GetEmpty());
        }
    }
}

void UMyPlayerHUDWidget::UpdateRoundTime(float RoundTimeValue)
{
    if (RoundTimeText)
    {
        int32 TotalSeconds = FMath::FloorToInt(RoundTimeValue);
        int32 Minutes = TotalSeconds / 60;
        int32 Seconds = TotalSeconds % 60;
        FString NewText = FString::Printf(TEXT("%d : %02d"), Minutes, Seconds);
        RoundTimeText->SetText(FText::FromString(NewText));
    }
}