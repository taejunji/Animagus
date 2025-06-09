// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerHUDWidget.h"
#include "Animation/WidgetAnimation.h"

#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h" // 나중에 스킬 목록 추가 시 사용
#include "Components/TextBlock.h"

void UMyPlayerHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SelectedOutlineColor = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f);
}

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

            // 초마다 색상 변경 ( 파스텔톤 - 빨 주 노 초 )
            FLinearColor NewColor;
            switch (static_cast<int32>(DisplayTime))
            {
            case 5:
                NewColor = FLinearColor(1.0f, 0.5f, 0.5f);
                break;
            case 4:
                NewColor = FLinearColor(1.0f, 0.521833f, 0.15625f);
                break;
            case 3:
                NewColor = FLinearColor(1.0f, 1.0f, 0.311012f);
                break;
            case 2:
                NewColor = FLinearColor(0.311012f, 1.0f, 0.400647f);
                break;
            case 1:
                NewColor = FLinearColor(0.6f, 0.8f, 1.0f);
                break;
            default:
                NewColor = FLinearColor(1.0f, 0.5f, 0.5f);
                break;
            }
             
            CountdownText->SetColorAndOpacity(FSlateColor(NewColor)); 
        }
        else
        {
            CountdownText->SetText(FText::GetEmpty());
            PlayAnimation(GameStartTime, 0.f, 1, EUMGSequencePlayMode::Forward, 2.f);
            // 2초짜리 1.333배 -> 1.5초 재생
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

        int32 Tens = Seconds / 10;   // 초의 십의 자리
        int32 Ones = Seconds % 10;   // 초의 일의 자리

        FString NewText = FString::Printf(TEXT("%d : %d %d"), Minutes, Tens, Ones);

        // FString NewText = FString::Printf(TEXT("%d : %02d"), Minutes, Seconds);
        RoundTimeText->SetText(FText::FromString(NewText));
    }
}

void UMyPlayerHUDWidget::UpdateSelectedSkillOutline(int32 SelectedIndex)
{
  
    if (SkillBorder_0)
    {
        SkillBorder_0->SetBrushColor((SelectedIndex == 0) ? SelectedOutlineColor : NormalOutlineColor);
    }
    if (SkillBorder_1)
    {
        SkillBorder_1->SetBrushColor((SelectedIndex == 1) ? SelectedOutlineColor : NormalOutlineColor);
    }
    if (SkillBorder_2)
    {
        SkillBorder_2->SetBrushColor((SelectedIndex == 2) ? SelectedOutlineColor : NormalOutlineColor);
    }
    if (SkillBorder_3)
    {
        SkillBorder_3->SetBrushColor((SelectedIndex == 3) ? SelectedOutlineColor : NormalOutlineColor);
    }
}

void UMyPlayerHUDWidget::PlayWidgetAnimation(UWidgetAnimation* WidgetAnimation, bool bLoop, float StartAtTime)
{
    if (WidgetAnimation)
    {
        PlayAnimation(WidgetAnimation, StartAtTime, bLoop ? 0 : 1);
        // 0 = infinite loop, 1 = once
        // NumLoopsToPlay = 0이면 애니메이션이 끝없이 반복
    }
}