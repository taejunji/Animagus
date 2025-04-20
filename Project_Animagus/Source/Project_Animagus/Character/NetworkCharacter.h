// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"

#include "../Server/Server/protocol.h"

#include "NetworkCharacter.generated.h"

/**
 *  패킷을 통해 "이동, 회전, 애니메이션" 등을 동기화하여 렌더링할 상대 플레이어
 */
UCLASS()
class PROJECT_ANIMAGUS_API ANetworkCharacter : public ABaseCharacter  
{
	GENERATED_BODY() 
	
public:
    ANetworkCharacter();
    virtual ~ANetworkCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    void SetPlayerType(Protocol::PlayerType type) { PlayerType = type; }
    void SetPlayerInfo(Protocol::PlayerInfo& info);
    void SetDestInfo(Protocol::PlayerInfo& info);


private:
    void InitPlayerMesh();

protected:
    Protocol::PlayerInfo* PlayerInfo; // 현재 위치
    Protocol::PlayerInfo* DestInfo;   // 목적지
};
