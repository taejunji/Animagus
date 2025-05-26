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
    void SetupWidget(float InTimeLimit, int32 InMaxSlots);

protected:
    virtual void NativeConstruct() override;

    // 1초마다 남은 시간 감소
    void TickTimer();

    // *선택 완료 버튼 클릭
    UFUNCTION()
    void OnConfirmClicked();

    // 선택 초기화 버튼 클릭 시
    UFUNCTION()
    void OnClearAllClicked();

    //** 아이콘 클릭 시 (Index는 리스트 내 위치)
    UFUNCTION()
    void OnSkillIconClicked(int32 IconIndex);

    
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

    // 로컬 타이머 핸들
    FTimerHandle LocalTimerHandle;

    // 남은 시간(초)
    float TimeRemaining;

    // 최대 슬롯 개수
    int32 MaxSlots;
public:
    // 스킬 클래스 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Selection")
    TArray<TSubclassOf<UBaseSkill>> AvailableClasses;

    // 선택한 스킬 클래스 여기 담긴거로 스킬 세팅ㅇㅇ
    TArray<TSubclassOf<UBaseSkill>> ChosenClasses;
};
