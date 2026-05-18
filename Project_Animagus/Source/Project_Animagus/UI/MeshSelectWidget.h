// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MeshSelectWidget.generated.h"

class UButton;
class USoundBase;

/** 캐릭터 메쉬 선택 UI 위젯 */
UCLASS()
class PROJECT_ANIMAGUS_API UMeshSelectWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 소유 컨트롤러를 설정 */
    UFUNCTION()
    void SetupOwner(class AConnectPlayerController* InController);

    void ActiveStartButton();

protected:
    virtual void NativeConstruct() override;

    // 12개 캐릭터 선택 버튼
    UPROPERTY(meta=(BindWidget)) UButton* BtnMonkey;
    UPROPERTY(meta=(BindWidget)) UButton* BtnKoala;
    UPROPERTY(meta=(BindWidget)) UButton* BtnSheep;
    UPROPERTY(meta=(BindWidget)) UButton* BtnFox;
    UPROPERTY(meta=(BindWidget)) UButton* BtnSloth;
    UPROPERTY(meta=(BindWidget)) UButton* BtnElephant;
    UPROPERTY(meta=(BindWidget)) UButton* BtnRaccoon;
    UPROPERTY(meta=(BindWidget)) UButton* BtnDeer;
    UPROPERTY(meta=(BindWidget)) UButton* BtnCow;
    UPROPERTY(meta=(BindWidget)) UButton* BtnUnicorn;
    UPROPERTY(meta=(BindWidget)) UButton* BtnZebra;
    UPROPERTY(meta=(BindWidget)) UButton* BtnDonkey;

    UPROPERTY(meta=(BindWidget)) UButton* BtnBack;
    UPROPERTY(meta=(BindWidget)) UButton* BtnNext;
    
    // 시작 버튼
    UPROPERTY(meta=(BindWidget)) UButton* BtnStart;

    // 효과음
    UPROPERTY(EditAnywhere, Category="Sound") USoundBase* HoverSound;
    UPROPERTY(EditAnywhere, Category="Sound") USoundBase* ClickSound;

private:
    TWeakObjectPtr<class AConnectPlayerController> Owner;

    // 선택 핸들러
    UFUNCTION() void HandleSelectMonkey();
    UFUNCTION() void HandleSelectKoala();
    UFUNCTION() void HandleSelectSheep();
    UFUNCTION() void HandleSelectFox();
    UFUNCTION() void HandleSelectSloth();
    UFUNCTION() void HandleSelectElephant();
    UFUNCTION() void HandleSelectRaccoon();
    UFUNCTION() void HandleSelectDeer();
    UFUNCTION() void HandleSelectCow();
    UFUNCTION() void HandleSelectUnicorn();
    UFUNCTION() void HandleSelectZebra();
    UFUNCTION() void HandleSelectDonkey();

    UFUNCTION() void HandleBackClicked();
    UFUNCTION() void HandleNextClicked();
    
    // 시작
    UFUNCTION() void HandleStartClicked();

    // 사운드 재생
    UFUNCTION() void PlayHoverSound();
    UFUNCTION() void PlayClickSound();
};
