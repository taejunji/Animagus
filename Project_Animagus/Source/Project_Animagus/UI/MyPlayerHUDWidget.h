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
};
