// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "SkillSlotWidget.generated.h"



UCLASS()
class PROJECT_ANIMAGUS_API USkillSlotWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION()
    void SetupSlot(int32 SlotIndex);

    UFUNCTION()
    void UpdateSlot(TSubclassOf<UBaseSkill> AssignedClass);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta=(BindWidget))
    UImage* SlotImage;
private:
    int32 Index;
	
};
