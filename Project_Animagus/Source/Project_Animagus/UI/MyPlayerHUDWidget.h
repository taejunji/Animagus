// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerHUDWidget.generated.h"

/**
 * 
 */

class UProgressBar;
class UImage;
class UHorizontalBox;
class UTextBlock;
class UBorder;
class UWidgetAnimation; 

UCLASS()
class PROJECT_ANIMAGUS_API UMyPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:

    UPROPERTY(meta = (BindWidget))
    UImage* AimImage;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* HPProgressBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CurrentHPText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextPlayerCount;

protected:
    UPROPERTY(EditAnywhere, Category="HUD")
    TArray<UTexture2D*> HUDTextures;
protected:

    // 시작 카운트다운을 표시할 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CountdownText;

    // 라운드 진행 시간을 표시할 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    UTextBlock* RoundTimeText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextRoundCount;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextMaxRoundCount;
protected:

    // 위젯 애니메이션
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* CountDownTime;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* GameStartTime;

protected:

    // 기본 윤곽선 색상과 선택되었을 때의 색상
    FLinearColor NormalOutlineColor = FLinearColor::Gray; 
    FLinearColor SelectedOutlineColor = FLinearColor::Yellow;

    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* SkillListBox;

protected:

    UPROPERTY(meta = (BindWidget))
    UImage* SkillImage0;
    
    UPROPERTY(meta = (BindWidget))
    UImage* SkillImage1;
    
    UPROPERTY(meta = (BindWidget))
    UImage* SkillImage2;
    
    UPROPERTY(meta = (BindWidget))
    UImage* SkillImage3;

protected:

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillCooldownProgressBar0;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillCooldownProgressBar1;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillCooldownProgressBar2;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillCooldownProgressBar3;

protected:

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SkillCoolDownTime_0;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SkillCoolDownTime_1;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SkillCoolDownTime_2;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SkillCoolDownTime_3;

protected:

    UPROPERTY(meta = (BindWidget))
    UBorder* SkillBorder_0;

    UPROPERTY(meta = (BindWidget))
    UBorder* SkillBorder_1;

    UPROPERTY(meta = (BindWidget))
    UBorder* SkillBorder_2;

    UPROPERTY(meta = (BindWidget))
    UBorder* SkillBorder_3;

protected:
    // 스킬 레벨 이미지 배열
    TArray<UImage*> ArrayLevel;

    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_0;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_1;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_2;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_3;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_4;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_5;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_6;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_7;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_8;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_9;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_10;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_11;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_12;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_13;
    UPROPERTY(meta = (BindWidget))
    UImage* Image_Level_14;

public:

    // 업데이트를 위해 외부에서 호출할 함수, HP 비율(0~1) 전달
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateHP(float HPPercent);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateAlivePlayerCount(int32 LivingPlayers);

    // 스킬 아이콘 업데이트 함수 (각 스킬 슬롯에 아이콘 이미지를 설정)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateSkillIcon(int32 SkillIndex, UTexture2D* NewTexture);

    // 스킬 쿨타임 진행률 업데이트 함수 (0~1 사이의 값)
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateSkillCooldown(int32 SkillIndex, float CooldownPercent);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateSkillCooldownTime(int32 SkillIndex, int32 CooldownTime);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateCountdown(float CountdownValue);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateRoundTime(float RoundTimeValue);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateSelectedSkillOutline(int32 SelectedIndex);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void PlayWidgetAnimation(UWidgetAnimation* WidgetAnimation, bool bLoop = false, float StartAtTime = 0.f);

    void SetCurrentHP(float hp, float max_hp);

    UWidgetAnimation* GetCountDownTime() const { return CountDownTime; }

    void ResetLevelImgage();

    void SetLevelImage(int32 num);

    void SetRoundText(int32 CurrentRound);
};
