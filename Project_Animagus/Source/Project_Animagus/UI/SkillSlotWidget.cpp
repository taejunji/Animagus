// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSlotWidget.h"

#include "SkillSelectionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Skill/BaseSkill.h"

// SkillSlotWidget.cpp
void USkillSlotWidget::NativeConstruct()
{
    if (EmptySlotTexture)
    {
        EmptySlotBrush.SetResourceObject(EmptySlotTexture);
        // 필요하다면 ImageSize도 설정 (예: 64×64 픽셀)
        EmptySlotBrush.ImageSize = FVector2D(
          100.f,100.f
        );
        
        // ✨ Alpha 0.5로 설정
        EmptySlotBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.5f));

        FButtonStyle NewStyle = SlotButton->GetStyle();
        NewStyle.SetNormal(EmptySlotBrush);
        NewStyle.SetHovered(EmptySlotBrush);
        NewStyle.SetPressed(EmptySlotBrush);
        SlotButton->SetStyle(NewStyle);
    }

    // 2) 버튼 클릭 바인딩
    if (SlotButton)
    {
        SlotButton->OnClicked.AddDynamic(this, &USkillSlotWidget::HandleSlotClicked);
        
        SlotButton->OnHovered.AddDynamic(this, &USkillSlotWidget::HandleSlotHovered);
    }
}

void USkillSlotWidget::SetupSlot(int32 SlotIndexIn)
{
    SlotIndex = SlotIndexIn;

}

void USkillSlotWidget::HandleSlotClicked()
{
    if (ClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ClickSound);
    }

    
    if (OwnerWidget.IsValid() && SlotIndex >= 0)
    {
        OwnerWidget->OnSlotClicked(SlotIndex);
    }
}

void USkillSlotWidget::HandleSlotHovered()
{
    if (HoverSound)
    {
        UGameplayStatics::PlaySound2D(this, HoverSound);
    }
}

void USkillSlotWidget::UpdateSlot(const TSubclassOf<UBaseSkill> AssignedClass) const
{
    if (!SlotButton)
        return;
    
    if (SlotIndex < 0) return;
    
    FButtonStyle NewStyle = SlotButton->GetStyle();  
    // 기존 스타일 복사

    if (AssignedClass)
    {
        // 1) 스킬 아이콘 텍스처를 이용해 새 브러시 생성
        UTexture2D* IconTex = AssignedClass->GetDefaultObject<UBaseSkill>()->GetIconTexture();
        if (IconTex)
        {
            FSlateBrush IconBrush;
            IconBrush.SetResourceObject(IconTex);
            IconBrush.ImageSize = FVector2D(
             100.f, 
             100.f
            );

            // 2) 버튼의 Normal/Hovered/Pressed 브러시 모두 스킬 아이콘으로 설정 (원하면 Hovered/Pressed는 살짝 Tint 강조)
            NewStyle.SetNormal(IconBrush);

            FSlateBrush HoverBrush = IconBrush;
            HoverBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.7f));
            NewStyle.SetHovered(HoverBrush);

            FSlateBrush PressedBrush = IconBrush;
            PressedBrush.TintColor = FSlateColor(FLinearColor(1.f, 0.8f, 0.8f, 0.5f));  

            NewStyle.SetPressed(PressedBrush);
        }
        else
        {
            // 아이콘이 없으면 빈 슬롯 브러시 사용
            NewStyle.SetNormal(EmptySlotBrush);
            NewStyle.SetHovered(EmptySlotBrush);
            NewStyle.SetPressed(EmptySlotBrush);
        }
    }
    else
    {
        if (EmptySlotTexture)
        {
            // 알파 0.5로 흐리게 표현
            FSlateBrush NormalBrush = EmptySlotBrush;
            NormalBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.5f)); // ✨ 알파 적용
            NewStyle.SetNormal(NormalBrush);

            FSlateBrush HoverBrush = EmptySlotBrush;
            HoverBrush.TintColor = FSlateColor(FLinearColor(0.9f, 0.9f, 0.9f, 1.f));
            NewStyle.SetHovered(HoverBrush);

            FSlateBrush PressedBrush = EmptySlotBrush;
            PressedBrush.TintColor = FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.f));
            NewStyle.SetPressed(PressedBrush);
        }
        else{
            // EmptySlotTexture도 없으면 완전 기본 스타일
            FSlateBrush WhiteBrush;
            WhiteBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.5f)); // ✨ 알파 적용
            WhiteBrush.ImageSize = FVector2D(100.f, 100.f);
            NewStyle.SetNormal(WhiteBrush);

            FSlateBrush HoverBrush = WhiteBrush;
            HoverBrush.TintColor = FSlateColor(FLinearColor(0.9f, 0.9f, 0.9f, 1.f));
            NewStyle.SetHovered(HoverBrush);

            FSlateBrush PressedBrush = WhiteBrush;
            PressedBrush.TintColor = FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f, 1.f));
            NewStyle.SetPressed(PressedBrush);
        }
    }
    // 3) 버튼 스타일 업데이트
    SlotButton->SetStyle(NewStyle);
}


