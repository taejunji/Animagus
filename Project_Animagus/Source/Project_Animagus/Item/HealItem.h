// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AHealItem : public ABaseItem
{
	GENERATED_BODY()
public:
    AHealItem();
    
    virtual void BeginPlay() override;
    virtual void OnPickedUp(class ABaseCharacter* Picker) override;
};
