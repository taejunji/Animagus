// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSelectionWidget.generated.h"


class ABattle_PlayerController;
class UBaseSkill;
class UTextBlock;
class UButton;
class UScrollBox;
class UHorizontalBox;
class USkillIconWidget;
class USkillSlotWidget;
UCLASS()
class PROJECT_ANIMAGUS_API USkillSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 컨트롤러 세팅
    UFUNCTION(BlueprintCallable, Category = "Skill Selection")
    void SetupOwner(ABattle_PlayerController* InController);

    // 초기화 
    UFUNCTION(BlueprintCallable, Category = "Skill Selection")
    void SetupWidget(float InTimeLimit);

public:
    virtual void NativeConstruct() override;
    
    // 슬롯 클릭 시 호출될 함수
    UFUNCTION()
    void OnSlotClicked(int32 SlotIndex);
    
    // *선택 완료 버튼 클릭
    UFUNCTION()
    void OnConfirmClicked();

    UFUNCTION()
    void AutoFillAndConfirm();

    // 선택 초기화 버튼 클릭 시
    UFUNCTION()
    void OnClearAllClicked();


    
    //** 아이콘 클릭 시 (Index는 리스트 내 위치)
    UFUNCTION()
    void OnSkillIconClicked(TSubclassOf<UBaseSkill> ClickedClass);

    UFUNCTION(BlueprintCallable, Category="Skill Selection")
    void UpdateTimerDisplay(int32 RemainingSeconds);

    UFUNCTION()
    void ForceConfirm() { OnConfirmClicked(); }
    
// 에디터에서 BP만들어서 설정
    
    // 남은 시간 표시 텍스트 
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerText;

    // Confirm 버튼 
    UPROPERTY(meta = (BindWidget))
    UButton* ConfirmButton;

    // Clear All 버튼 
    UPROPERTY(meta = (BindWidget))
    UButton* ClearAllButton;

    // 스킬 아이콘 리스트 
    UPROPERTY(meta = (BindWidget))
    UScrollBox* SkillIconList;

    // 선택 슬롯 컨테이너
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* SlotContainer;

private:
    // 이 위젯을 띄운 컨트롤러 포인터
    TWeakObjectPtr<ABattle_PlayerController> OwnerController;

    // 최대 슬롯 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill Selection", meta=(AllowPrivateAccess))
    int32 MaxSlots = 4;
public:
    // 스킬 클래스 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Selection")
    TArray<TSubclassOf<UBaseSkill>> AvailableClasses;

    // 선택한 스킬 클래스 여기 담긴거로 스킬 세팅ㅇㅇ
    TArray<TSubclassOf<UBaseSkill>> ChosenClasses;

    /** 스킬 아이콘 위젯 클래스 (BP에서 지정) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Selection")
    TSubclassOf<USkillIconWidget> SkillIconWidgetClass;

    /** 스킬 슬롯 위젯 클래스 (BP에서 지정) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Selection")
    TSubclassOf<USkillSlotWidget> SkillSlotWidgetClass;
};


