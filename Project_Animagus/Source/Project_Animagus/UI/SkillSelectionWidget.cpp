// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSelectionWidget.h"

#include "SkillIconWidget.h"
#include "SkillSlotWidget.h"
#include "../GameMode/BattleGameMode.h"
#include "../Skill/BaseSkill.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "TimerManager.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/SizeBox.h"
#include "Project_Animagus/PlayerController/Battle_PlayerController.h"

void USkillSelectionWidget::NativeConstruct()
{
  Super::NativeConstruct();
    
    SlotContainer->ClearChildren();
    for (int32 i = 0; i < MaxSlots; ++i)
    {
        USkillSlotWidget* SkillSlot = CreateWidget<USkillSlotWidget>(this, SkillSlotWidgetClass);
        SkillSlot->SetupSlot(i);

        SkillSlot->OwnerWidget = this;
        
        SkillSlot->UpdateSlot(nullptr);
        
        UHorizontalBoxSlot* HSlot = SlotContainer->AddChildToHorizontalBox(SkillSlot);
        
        HSlot->SetPadding(FMargin(50.f, 0.f, 50.f, 0.f));
    }

   
    SkillIconList->ClearChildren();
    for (int32 i = 0; i < AvailableClasses.Num(); ++i)
    {
        USkillIconWidget* Icon = CreateWidget<USkillIconWidget>(this, SkillIconWidgetClass);
        Icon->SetupIcon(AvailableClasses[i], this);

        // 1) AddChild(...) 호출해서 UPanelSlot* 반환
        UPanelSlot* PanelSlot = SkillIconList->AddChild(Icon);
        if (!PanelSlot)
            continue;

        // 2) UPanelSlot을 UScrollBoxSlot으로 캐스트
        UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot>(PanelSlot);
        if (ScrollSlot)
        {
            // 3) 패딩 설정 (Left=5, Top=5, Right=5, Bottom=5)
            ScrollSlot->SetPadding(FMargin(5.f, 5.f));
        }
    }
    
  
  if (ConfirmButton)
    ConfirmButton->OnClicked.AddDynamic(this, &USkillSelectionWidget::OnConfirmClicked);

  if (ClearAllButton)
    ClearAllButton->OnClicked.AddDynamic(this, &USkillSelectionWidget::OnClearAllClicked);


}

void USkillSelectionWidget::OnSlotClicked(int32 SlotIndex)
{
    UE_LOG(LogTemp, Log, TEXT("SelectionWidget: Slot %d clicked"), SlotIndex);

   
    if (SlotIndex < ChosenClasses.Num())
    {
        TSubclassOf<UBaseSkill> Removed = ChosenClasses[SlotIndex];
        ChosenClasses.RemoveAt(SlotIndex);
        UE_LOG(LogTemp, Log, TEXT("SelectionWidget: Removed %s from ChosenClasses"), *Removed->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("SelectionWidget: Slot %d was empty, nothing to remove"), SlotIndex);
    }

    
    int32 SlotCount = SlotContainer->GetChildrenCount();
    for (int32 i = 0; i < SlotCount; ++i)
    {
        USkillSlotWidget* SkillSlot = Cast<USkillSlotWidget>(SlotContainer->GetChildAt(i));
        TSubclassOf<UBaseSkill> Assigned = (i < ChosenClasses.Num()) ? ChosenClasses[i] : nullptr;
        if (SkillSlot)
            SkillSlot->UpdateSlot(Assigned);
    }
}

void USkillSelectionWidget::SetupOwner(ABattle_PlayerController* InController)
{
  OwnerController = InController;
}

void USkillSelectionWidget::SetupWidget(float InTimeLimit)
{
    
  // 초기 UI 업데이트
  if (TimerText)
  {
      // TimerText->SetText(FText::AsNumber(FMath::CeilToInt()));
  }
}

void USkillSelectionWidget::UpdateTimerDisplay(int32 RemainingSeconds)
{
    if (TimerText)
    {
        TimerText->SetText(FText::AsNumber(RemainingSeconds));
    }
}

void USkillSelectionWidget::OnConfirmClicked()
{
  // 완료 버튼 

  // 컨트롤러에게 직접 호출
  if (OwnerController.IsValid())
  {
   OwnerController->OnSkillSelectionConfirmed(ChosenClasses);
  }

  // 위젯 제거 및 타이머 정리
  RemoveFromParent();
}

void USkillSelectionWidget::AutoFillAndConfirm()
{
    if (!SlotContainer)
    {
        UE_LOG(LogTemp, Warning, TEXT("AutoFillAndConfirm: SlotContainer is null"));
        return;
    }

    // 1) 아직 채워지지 않은 칸 수 계산
    int32 CurrentCount = ChosenClasses.Num();
    int32 ToFill = MaxSlots - CurrentCount;
    UE_LOG(LogTemp, Log, TEXT("AutoFillAndConfirm: Current=%d, ToFill=%d"), CurrentCount, ToFill);

    if (ToFill > 0)
    {
        // 2) AvailableClasses 중 미선택 클래스만 Candidates에 수집
        TArray<TSubclassOf<UBaseSkill>> Candidates;
        for (auto& Cl : AvailableClasses)
        {
            if (!ChosenClasses.Contains(Cl))
                Candidates.Add(Cl);
        }

        if (Candidates.Num() > 1)
        {
            for (int32 i = 0; i < Candidates.Num(); ++i)
            {
                int32 SwapIdx = FMath::RandRange(i, Candidates.Num() - 1);
                Candidates.Swap(i, SwapIdx);
            }
        }
        
        // 3) 남은 슬롯 수만큼 Candidates에서 차례로 추가
        for (int32 i = 0; i < ToFill && Candidates.IsValidIndex(i); ++i)
        {
            ChosenClasses.Add(Candidates[i]);
        }

        // 4) 슬롯 UI에 반영
        int32 ChildCount = SlotContainer->GetChildrenCount();
        for (int32 i = 0; i < ChildCount && i < MaxSlots; ++i)
        {
            // GetChildAt(i)가 실제로 SkillSlotWidget인지 검사
            UWidget* Child = SlotContainer->GetChildAt(i);
            USkillSlotWidget* skillSlot = Cast<USkillSlotWidget>(Child);
            if (!skillSlot)
            {
                UE_LOG(LogTemp, Warning, TEXT("AutoFillAndConfirm: child %d is not USkillSlotWidget"), i);
                continue;
            }

            // ChosenClasses에 의해 할당할 클래스 결정
            TSubclassOf<UBaseSkill> Assigned = (i < ChosenClasses.Num())
                ? ChosenClasses[i]
                : nullptr;

            skillSlot->UpdateSlot(Assigned);
        }
    }
    
    ForceConfirm();
}

void USkillSelectionWidget::OnClearAllClicked()
{
  ChosenClasses.Empty();
    
    int32 SlotCount = SlotContainer->GetChildrenCount();
    for (int32 i = 0; i < SlotCount; ++i)
    {
        USkillSlotWidget* SkillSlot = Cast<USkillSlotWidget>(SlotContainer->GetChildAt(i));
        if (SkillSlot)
        {
            SkillSlot->UpdateSlot(nullptr);
        }
    }
}


void USkillSelectionWidget::OnSkillIconClicked(TSubclassOf<UBaseSkill> ClickedClass)
{
    if (!ClickedClass)
    {
        UE_LOG(LogTemp, Log, TEXT("-- OnSkillIconClicked returned"));
        return;
    } 

    UE_LOG(LogTemp, Log, TEXT("-- OnSkillIconClicked START --"));
    UE_LOG(LogTemp, Log, TEXT("ClickedClass: %s"), *ClickedClass->GetName());
    
    // 이미 선택된 클래스인지 확인
    if (ChosenClasses.Contains(ClickedClass))
    {
        ChosenClasses.Remove(ClickedClass);
        UE_LOG(LogTemp, Log, TEXT("Removed from ChosenClasses"));
    }
    else if (ChosenClasses.Num() < MaxSlots)
    {
        // 아직 슬롯 여유가 있으면 추가
        ChosenClasses.Add(ClickedClass);
        UE_LOG(LogTemp, Log, TEXT("Added to ChosenClasses"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("MaxSlots reached, cannot add"));
    }

    UE_LOG(LogTemp, Log, TEXT("ChosenClasses count: %d"), ChosenClasses.Num());

    for (int32 i = 0; i < ChosenClasses.Num(); ++i)
    {
        UE_LOG(LogTemp, Log, TEXT("  ChosenClasses[%d] = %s"), i, *ChosenClasses[i]->GetName());
    }
    
    
    // 슬롯 위젯에 반영
    int32 SlotCount = SlotContainer->GetChildrenCount();
    UE_LOG(LogTemp, Log, TEXT("SlotContainer has %d children"), SlotCount);

    
    for (int32 i = 0; i < SlotCount; ++i)
    {
        USkillSlotWidget* SkillSlot = Cast<USkillSlotWidget>(SlotContainer->GetChildAt(i));
        if (SkillSlot)
        {
            TSubclassOf<UBaseSkill> Assigned = (i < ChosenClasses.Num())
              ? ChosenClasses[i]
              : nullptr;

            UE_LOG(LogTemp, Log, TEXT("Updating Slot %d with %s"),
            i,
            Assigned ? *Assigned->GetName() : TEXT("NULL"));
            
            SkillSlot->UpdateSlot(Assigned);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("-- OnSkillIconClicked END --"));
}
