// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Server/Server/protocol.h"
#include "LoginGameMode.generated.h"



UCLASS()
class PROJECT_ANIMAGUS_API ALoginGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    ALoginGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void HandleLoginSuccess();
    void HandleLoginFail(Protocol::SC_LOGIN_FAIL_PKT& pkt);
    void HandleSignUpSuccess();
    void HandleSignUpFail();
};

