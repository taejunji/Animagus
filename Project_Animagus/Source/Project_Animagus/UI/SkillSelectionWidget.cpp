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
#include "Project_Animagus/PlayerController/Battle_PlayerController.h"

void USkillSelectionWidget::NativeConstruct()
{
  Super::NativeConstruct();

    // 1) 슬롯 위젯을 MaxSlots 개수만큼 생성
    SlotContainer->ClearChildren();
    for (int32 i = 0; i < MaxSlots; ++i)
    {
        USkillSlotWidget* SkillSlot = CreateWidget<USkillSlotWidget>(this, SkillSlotWidgetClass);
        SkillSlot->SetupSlot(i);
        SlotContainer->AddChild(SkillSlot);
    }

    // 2) 스킬 아이콘 리스트 뿌리기
    SkillIconList->ClearChildren();
    for (int32 i = 0; i < AvailableClasses.Num(); ++i)
    {
        USkillIconWidget* Icon = CreateWidget<USkillIconWidget>(this, SkillIconWidgetClass);
        Icon->SetupIcon(AvailableClasses[i]);
        // 인덱스를 전달할 수 있으면, SetupIcon에 추가 파라미터로 넘겨도 좋음
        SkillIconList->AddChild(Icon);
    }
    
  // 버튼 바인딩
  if (ConfirmButton)
    ConfirmButton->OnClicked.AddDynamic(this, &USkillSelectionWidget::OnConfirmClicked);

  if (ClearAllButton)
    ClearAllButton->OnClicked.AddDynamic(this, &USkillSelectionWidget::OnClearAllClicked);

  
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            LocalTimerHandle, this, &USkillSelectionWidget::TickTimer, 1.f, true);
    }

}

void USkillSelectionWidget::SetupOwner(ABattle_PlayerController* InController)
{
  OwnerController = InController;
}

void USkillSelectionWidget::SetupWidget(float InTimeLimit)
{
  TimeRemaining = InTimeLimit;


  // 타이머 시작
  if (GetWorld())
  {
    GetWorld()->GetTimerManager().SetTimer(
      LocalTimerHandle,
      this,
      &USkillSelectionWidget::TickTimer,
      1.0f,
      true
    );
  }

  // 초기 UI 업데이트
  if (TimerText)
    TimerText->SetText(FText::AsNumber(FMath::CeilToInt(TimeRemaining)));

  // TODO: AvailableClasses 를 세팅한 뒤,
  // SkillIconList 에 아이콘 위젯을 생성해서 표시하세요.
}

void USkillSelectionWidget::TickTimer()
{
  if (--TimeRemaining <= 0)
  {
    OnConfirmClicked();
    return;
  }

  if (TimerText)
    TimerText->SetText(FText::AsNumber(FMath::CeilToInt(TimeRemaining)));
}

void USkillSelectionWidget::OnConfirmClicked()
{
  // 완료 버튼 

  // 컨트롤러에게 직접 호출
  if (OwnerController.IsValid())
  {
   // OwnerController->OnSkillSelectionConfirmed(ChosenClasses);
  }

  // 위젯 제거 및 타이머 정리
  RemoveFromParent();
  if (GetWorld())
    GetWorld()->GetTimerManager().ClearTimer(LocalTimerHandle);
}

void USkillSelectionWidget::OnClearAllClicked()
{
  ChosenClasses.Empty();
  // TODO: SlotContainer 내 슬롯 UI 리셋
}

void USkillSelectionWidget::OnSkillIconClicked(TSubclassOf<UBaseSkill> ClickedClass)
{
    if (!ClickedClass) 
        return;

    // 이미 선택된 클래스인지 확인
    if (ChosenClasses.Contains(ClickedClass))
    {
        // 선택 해제
        ChosenClasses.Remove(ClickedClass);
    }
    else if (ChosenClasses.Num() < MaxSlots)
    {
        // 아직 슬롯 여유가 있으면 추가
        ChosenClasses.Add(ClickedClass);
    }

    // 슬롯 위젯에 반영
    int32 SlotCount = SlotContainer->GetChildrenCount();
    for (int32 i = 0; i < SlotCount; ++i)
    {
        USkillSlotWidget* SkillSlot = Cast<USkillSlotWidget>(SlotContainer->GetChildAt(i));
        if (SkillSlot)
        {
            TSubclassOf<UBaseSkill> Assigned = (i < ChosenClasses.Num())
              ? ChosenClasses[i]
              : nullptr;
            SkillSlot->UpdateSlot(Assigned);
        }
    }
    
}