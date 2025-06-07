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
class USkillSelectionWidget;
class UBaseSkill;
class USoundBase;
class UTextBlock;

UCLASS()
class PROJECT_ANIMAGUS_API USkillIconWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    // 슬롯으로 드래그·드롭 전이나 클릭 시 사용할 스킬 클래스
    UPROPERTY(BlueprintReadWrite, Category="Skill")
    TSubclassOf<UBaseSkill> SkillClass;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DescriptionText;

    // 클릭 바인딩
    UFUNCTION()
    void OnIconClicked();

private:
    TWeakObjectPtr<USkillSelectionWidget> OwnerWidget;

public:
    void SetupIcon(TSubclassOf<UBaseSkill> InClass, USkillSelectionWidget* InOwner);
    
protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void HandleClicked();

    /** 버튼에 마우스를 올렸을 때 호출되는 함수 */
    UFUNCTION()
    void HandleHovered();
    
    
    UPROPERTY(meta=(BindWidget))
    UButton* IconButton;

    /** Hover 시 재생할 효과음 */
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    /** Click 시 재생할 효과음 */
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* ClickSound;
};
