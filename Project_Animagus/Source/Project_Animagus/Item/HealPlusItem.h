// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealPlusItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AHealPlusItem : public ABaseItem
{
	GENERATED_BODY()
public:
    AHealPlusItem();
    
    virtual void BeginPlay() override;
    virtual void OnPickedUp(class ABaseCharacter* Picker) override;	
};
