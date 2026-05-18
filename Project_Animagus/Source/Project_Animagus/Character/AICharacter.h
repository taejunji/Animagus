// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "AICharacter.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;

public:
    
    virtual void ApplyStun(float Duration);

    virtual void RemoveStun();

    void JumpAI();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI") 
    float RetreatThreshold = 20.f;  // 캐릭터마다 블루프린트에서 설정 가능 

private:
    void InitPlayerMesh();

public:
    void SetPlayerMesh(Protocol::PlayerType MeshType) { PlayerType = MeshType; InitPlayerMesh(); }
};
