// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillIconWidget.h"
#include "SkillSelectionWidget.h"
#include "Components/Button.h"
#include "../Skill/BaseSkill.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Skill/BaseSkill.h"


void USkillIconWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (IconButton)
    {
        // 1) 클릭 바인딩
        IconButton->OnClicked.AddDynamic(this, &USkillIconWidget::HandleClicked);

        // 2) Hover 바인딩
        IconButton->OnHovered.AddDynamic(this, &USkillIconWidget::HandleHovered);
        
    }
    
    if (DescriptionText)
    {
        DescriptionText->SetVisibility(ESlateVisibility::Visible);
    }
}

void USkillIconWidget::HandleClicked()
{
    if (ClickSound)
    {
        UGameplayStatics::PlaySound2D(this, ClickSound);
    }

    if (SkillClass && OwnerWidget.IsValid())
    {
        OwnerWidget->OnSkillIconClicked(SkillClass);
    }
    
}

void USkillIconWidget::HandleHovered()
{
    if (HoverSound)
    {
        UGameplayStatics::PlaySound2D(this, HoverSound);
    }
    
}

void USkillIconWidget::SetupIcon(TSubclassOf<UBaseSkill> InClass, USkillSelectionWidget* InOwner)
{
    SkillClass = InClass;
    OwnerWidget = InOwner;

    if (!IconButton || !SkillClass) return;


    
    // 1) 기본 아이콘 브러시 생성
    UTexture2D* IconTex = SkillClass->GetDefaultObject<UBaseSkill>()->GetIconTexture();
    if (IconTex)
    {
        FSlateBrush IconBrush;
        IconBrush.SetResourceObject(IconTex);
        IconBrush.ImageSize = FVector2D(100.f, 100.f);
        

        // 2) 버튼 Style을 복사해서 기본, Hovered, Pressed 브러시 설정
        FButtonStyle NewStyle = IconButton->GetStyle();
        NewStyle.SetNormal(IconBrush);

        // Hovered: 살짝 투명 Tint 적용
        FSlateBrush HoverBrush = IconBrush;
        HoverBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.7f));
        NewStyle.SetHovered(HoverBrush);

        // Pressed: 살짝 붉은 Tint 적용
        FSlateBrush PressedBrush = IconBrush;
        PressedBrush.TintColor = FSlateColor(FLinearColor(1.f, 0.8f, 0.8f, 0.5f));
        NewStyle.SetPressed(PressedBrush);

        IconButton->SetStyle(NewStyle);
    }
    else
    {
        // 아이콘 텍스처가 없으면 버튼을 숨기거나 기본 브러시로 처리
        IconButton->SetVisibility(ESlateVisibility::Hidden);
    }
    
    if (DescriptionText)
    {
        // UBaseSkill의 SkillDescription 문자열을 가져와 TextBlock에 설정
        FString Desc = SkillClass->GetDefaultObject<UBaseSkill>()->SkillDescription;
        DescriptionText->SetText(FText::FromString(Desc));
        DescriptionText->SetVisibility(ESlateVisibility::Visible);
    }
}




void USkillIconWidget::OnIconClicked()
{
    if (!SkillClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SkillIconWidget: Clicked but SkillClass is null"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("SkillIconWidget: Clicked %s"), *SkillClass->GetName());
    // 부모 위젯에 전달
    if (OwnerWidget.IsValid())
    {
        OwnerWidget->OnSkillIconClicked(SkillClass);
    }
}
