// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "PowerUpItem.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API APowerUpItem : public ABaseItem
{
	GENERATED_BODY()

public:
    APowerUpItem();

    /** 아이템 획득 시 처리 로직 오버라이드 */
    virtual void OnPickedUp(class ABaseCharacter* Picker) override;
};
