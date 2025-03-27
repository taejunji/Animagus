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
    virtual void Tick(float DeltaTime) override;

    void SetPlayerID(uint16 p_id) { player_id = p_id; };
    void SetPlayerType(Protocol::PlayerType type) { PlayerType = type; }

protected:
    virtual void BeginPlay() override;

private:
    void InitPlayerMesh();
};
