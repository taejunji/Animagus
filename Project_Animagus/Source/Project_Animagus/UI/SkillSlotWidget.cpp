// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSlotWidget.h"
#include "Project_Animagus/Skill/BaseSkill.h"

// SkillSlotWidget.cpp
void USkillSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // (필요시) 클릭 이벤트 추가
}
void USkillSlotWidget::SetupSlot(int32 SlotIndexIn)
{
    Index = SlotIndexIn;
    // 초기 빈 슬롯 비주얼 처리
    SlotImage->SetBrushFromTexture(nullptr);
}

void USkillSlotWidget::UpdateSlot(TSubclassOf<UBaseSkill> AssignedClass)
{
    if (AssignedClass)
        SlotImage->SetBrushFromTexture(
          AssignedClass->GetDefaultObject<UBaseSkill>()->GetIconTexture());
    else
        SlotImage->SetBrushFromTexture(nullptr);
}
