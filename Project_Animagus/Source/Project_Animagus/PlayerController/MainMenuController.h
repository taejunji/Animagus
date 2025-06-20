// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

class UMainMenuWidget;

UCLASS()
class PROJECT_ANIMAGUS_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    /** Start 버튼 클릭 */
    UFUNCTION() void OnStartClicked();

    /** Settings 버튼 클릭 */
    UFUNCTION() void OnSettingsClicked();

protected:
    /** Widget Blueprint 에 설정할 클래스 */
    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

    /** 생성된 위젯 인스턴스 */
    UPROPERTY()
    UMainMenuWidget* MainMenuWidget;
};
