// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
enum class LevelType { Login, Lobby, Battle };

UCLASS()
class PROJECT_ANIMAGUS_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    UMyGameInstance();

    // 현재 레벨 타입
    LevelType current_level;

    // 라운드 카운트
    int32 round_count;

public:
    void InitGameInstance();
    void PrintGameInstanceData();

    void SwitchLevel(LevelType level);  
    void SetLevelType(LevelType level) { current_level = level; }
    void SetRoundCount(int32 new_round_count) { round_count = new_round_count; }

    LevelType GetLevelType() const { return current_level; }
    int32 GetRoundCount() const { return round_count; }
};
