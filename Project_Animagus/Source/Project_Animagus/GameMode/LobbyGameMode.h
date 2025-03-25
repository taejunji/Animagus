// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    ALobbyGameMode();

    // 로비 레벨에서 사용할 HUD 위젯 클래스 (에디터에서 할당)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> LobbyHUDClass;
};
