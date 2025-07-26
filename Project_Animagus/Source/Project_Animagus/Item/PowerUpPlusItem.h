// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "PowerUpPlusItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API APowerUpPlusItem : public ABaseItem
{
	GENERATED_BODY()

public:
    APowerUpPlusItem();

    virtual void BeginPlay() override;

    /** 아이템 획득 시 처리 로직 오버라이드 */
    virtual void OnPickedUp(class ABaseCharacter* Picker) override;	
};
