// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillIconWidget.h"
#include "SkillSelectionWidget.h"
#include "Components/Button.h"
#include "Project_Animagus/Skill/BaseSkill.h"

// SkillIconWidget.cpp
void USkillIconWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (UButton* Btn = Cast<UButton>(GetWidgetFromName(TEXT("IconButton"))))
        Btn->OnClicked.AddDynamic(this, &ThisClass::OnIconClicked);
}
void USkillIconWidget::SetupIcon(TSubclassOf<UBaseSkill> InClass)
{
    SkillClass = InClass;
    if (IconImage && SkillClass)
    {
        auto Default = SkillClass->GetDefaultObject<UBaseSkill>();
        IconImage->SetBrushFromTexture(Default->GetIconTexture());
    }
}
void USkillIconWidget::OnIconClicked()
{
    // 부모 위젯에 전달
    if (USkillSelectionWidget* Parent = Cast<USkillSelectionWidget>(GetOuter()))
        Parent->OnSkillIconClicked(SkillClass);
}
