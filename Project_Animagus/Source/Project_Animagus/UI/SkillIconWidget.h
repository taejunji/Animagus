// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "SkillIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API USkillIconWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    // 슬롯으로 드래그·드롭 전이나 클릭 시 사용할 스킬 클래스
    UPROPERTY(BlueprintReadWrite, Category="Skill")
    TSubclassOf<UBaseSkill> SkillClass;

    // 이 아이콘 위젯을 초기화하는 함수
    UFUNCTION()
    void SetupIcon(TSubclassOf<UBaseSkill> InClass);

    // 클릭 바인딩
    UFUNCTION()
    void OnIconClicked();

protected:
    virtual void NativeConstruct() override;

    /** 이미지 바인딩 */ 
    UPROPERTY(meta=(BindWidget))
    UImage* IconImage;
    
    UPROPERTY(meta=(BindWidget))
    UButton* IconButton;
};
