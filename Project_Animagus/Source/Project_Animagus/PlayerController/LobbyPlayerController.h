// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaTime) override;

protected:
    // 로비 UI 위젯 클래스 (에디터에서 할당)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> LobbyHUDClass;

    // 생성된 로비 UI 위젯 인스턴스
    UPROPERTY()
    class UUserWidget* LobbyHUD;
};
