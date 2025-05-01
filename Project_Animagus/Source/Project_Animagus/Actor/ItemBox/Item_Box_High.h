// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Box_Base.h"
#include "Item_Box_High.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AItem_Box_High : public AItem_Box_Base
{
	GENERATED_BODY()
public:
    AItem_Box_High();

protected:
    
    // 오버라이드된 BreakBox 함수: 다른 Mesh/GC, 그리고 스폰 로직을 처리
    virtual void BreakBox() override;
};
