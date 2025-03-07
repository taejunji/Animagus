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
	
public:
    virtual void ApplyStun(float Duration);

    virtual void RemoveStun();
};
