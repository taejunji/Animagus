// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillSelectionWidget.h"
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

  // 버튼 바인딩
  if (ConfirmButton)
    ConfirmButton->OnClicked.AddDynamic(this, &USkillSelectionWidget::OnConfirmClicked);

  if (ClearAllButton)
    ClearAllButton->OnClicked.AddDynamic(this, &USkillSelectionWidget::OnClearAllClicked);

  
  // SkillIconWidget / SkillSlotWidget 을 생성하고,
  // 각각 클릭 시 OnSkillIconClicked(Index) 호출하도록 바인딩

}

void USkillSelectionWidget::SetupOwner(ABattle_PlayerController* InController)
{
  OwnerController = InController;
}

void USkillSelectionWidget::Initialize(float InTimeLimit, int32 InMaxSlots)
{
  TimeRemaining = InTimeLimit;
  MaxSlots = InMaxSlots;

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

void USkillSelectionWidget::OnSkillIconClicked(int32 IconIndex)
{
  if (!AvailableClasses.IsValidIndex(IconIndex))
    return;

  TSubclassOf<UBaseSkill> Clicked = AvailableClasses[IconIndex];
  if (ChosenClasses.Contains(Clicked))
    ChosenClasses.Remove(Clicked);
  else if (ChosenClasses.Num() < MaxSlots)
    ChosenClasses.Add(Clicked);

  // TODO: SlotContainer 내 슬롯 위젯에 ChosenClasses 반영
}