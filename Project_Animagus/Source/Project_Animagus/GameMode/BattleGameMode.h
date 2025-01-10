// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    virtual void StartPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
	ABattleGameMode();
	
private:
    // 라운드 경과 시간 출력
    FTimerHandle battle_timer_handle;
    float elasped_time;

    void InitBattleMode();
    void PrintElapsedtime();
};
