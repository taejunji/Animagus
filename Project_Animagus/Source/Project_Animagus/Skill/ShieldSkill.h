// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "ShieldSkill.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECT_ANIMAGUS_API UShieldSkill : public UBaseSkill
{
    GENERATED_BODY()

public:
    UShieldSkill();

    // 보호막 액터의 블루프린트 클래스 (에디터에서 할당)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShieldSkill")
    TSubclassOf<class AShieldActor> ShieldActorBPClass;

    // 보호막 지속 시간 (스킬 효과 지속 시간, 쿨타임과 별도)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ShieldSkill")
    float ShieldDuration;

    // 보호막 스킬 발동 시 보호막 액터를 소환하는 함수
    virtual void ActiveSkill_Implementation() override;
};
