// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultWidget.generated.h"


class UButton;
class UTextBlock;
class USoundBase;

UCLASS()
class PROJECT_ANIMAGUS_API UResultWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    
    UFUNCTION()
    void ShowOutcome(bool bIsWinner);
    
    virtual void NativeConstruct() override;

    // 승리 패배 출력
    UPROPERTY(meta=(BindWidget))
    UTextBlock* TxtOutcome;

    // 확인 버튼 오픈레벨 메인메뉴로
    UPROPERTY(meta=(BindWidget))
    UButton* BtnConfirm;

    UFUNCTION()
    void OnConfirmClicked();

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* ClickSound;

    UFUNCTION()
    void PlayHoverSound();
    
};
