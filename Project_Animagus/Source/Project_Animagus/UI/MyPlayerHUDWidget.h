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


UCLASS()
class PROJECT_ANIMAGUS_API UMyPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    // 업데이트를 위해 외부에서 호출할 함수, HP 비율(0~1) 전달
    UFUNCTION(BlueprintCallable, Category="UI")
    void UpdateHP(float HPPercent);

    // 스킬 아이콘 업데이트 함수 (각 스킬 슬롯에 아이콘 이미지를 설정)
    UFUNCTION(BlueprintCallable, Category="UI")
    void UpdateSkillIcon(int32 SkillIndex, UTexture2D* NewTexture);

    // 스킬 쿨타임 진행률 업데이트 함수 (0~1 사이의 값)
    UFUNCTION(BlueprintCallable, Category="UI")
    void UpdateSkillCooldown(int32 SkillIndex, float CooldownPercent);
    
protected:
    // 화면 중앙에 고정될 에임 이미지
    UPROPERTY(meta = (BindWidget))
    UImage* AimImage;

    // 캐릭터 HP 상태를 표시할 ProgressBar
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HPProgressBar;

    // 장착한 스킬 목록 및 쿨타임 표시용 위젯 (나중에 추가할 예정)
     UPROPERTY(meta = (BindWidget))
     UHorizontalBox* SkillListBox;

    // 스킬 아이콘 4개 (각 슬롯에 해당하는 아이콘)
    UPROPERTY(meta = (BindWidget))
    UImage* SkillImage0;
    
    UPROPERTY(meta = (BindWidget))
    UImage* SkillImage1;
    
    UPROPERTY(meta = (BindWidget))
    UImage* SkillImage2;
    
    UPROPERTY(meta = (BindWidget))
    UImage* SkillImage3;

    // 각 스킬 슬롯의 쿨타임을 나타내는 ProgressBar (작은 크기로 배치)
    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillCooldownProgressBar0;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillCooldownProgressBar1;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillCooldownProgressBar2;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* SkillCooldownProgressBar3;
};
