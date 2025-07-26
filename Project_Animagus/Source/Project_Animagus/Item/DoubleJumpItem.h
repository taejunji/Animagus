// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "DoubleJumpItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API ADoubleJumpItem : public ABaseItem
{
	GENERATED_BODY()

public:
    
    ADoubleJumpItem();

    virtual void BeginPlay() override;
    virtual void OnPickedUp(class ABaseCharacter* Picker) override;
};
