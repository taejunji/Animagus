// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ConnectPlayerController.generated.h"

class UUConnectWidget;

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AConnectPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;

protected:
    /** 블루프린트로 만든 WBP_Connect 위젯 클래스 */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> ConnectWidgetClass;

    /** 생성된 위젯 인스턴스 */
    UPROPERTY()
    UUConnectWidget* ConnectWidget;

    /** 버튼 클릭 핸들러 */
    UFUNCTION()
    void OnConnectButtonClicked();
};
