// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "SkillSlotWidget.generated.h"


class USoundBase;

UCLASS()
class PROJECT_ANIMAGUS_API USkillSlotWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION()
    void SetupSlot(int32 SlotIndex);

    UFUNCTION()
    void UpdateSlot(const TSubclassOf<UBaseSkill> AssignedClass) const;

    UFUNCTION()
    void HandleSlotClicked();

    UFUNCTION()
    void HandleSlotHovered();
    
protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* SlotButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill Slot")
    UTexture2D* EmptySlotTexture;
    
    FSlateBrush EmptySlotBrush;
private:
    int32 SlotIndex = -1;
public:
    /** 슬롯 클릭 시 재생할 사운드 */
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* ClickSound;

    /** 슬롯 호버 시 재생할 사운드 */
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    TWeakObjectPtr<class USkillSelectionWidget> OwnerWidget;
};
