// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RoomSelectGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API ARoomSelectGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    ARoomSelectGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    void HandleRoomEnter();
    void HandleRoomEnterFail();
};


