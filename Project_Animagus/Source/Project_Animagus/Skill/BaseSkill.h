// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseSkill.generated.h"

/**
 *  스킬 기본 베이스 - 공통 작업
 */
UCLASS()
class PROJECT_ANIMAGUS_API UBaseSkill : public UObject
{
	GENERATED_BODY()

public:

    UPROPERTY(BlueprintReadWrite, Category = "Skill")
    FString skill_name; // 스킬 이름

    UPROPERTY(BlueprintReadWrite, Category = "Skill")
    FString skill_description; // 스킬 설명

    UPROPERTY(BlueprintReadWrite, Category = "Skill")
    TObjectPtr<class ABaseCharacter> owner; // 소유자

    UPROPERTY(BlueprintReadWrite, Category = "Skill")
    float cooldown_time; // 스킬 쿨타임 시간 

public:
    virtual void ActiveSkill();   // 스킬 활성화
    virtual void DeactiveSkill(); // 스킬 비활성화
	
    // 쿨타임
    virtual void StartCoolDown();
    virtual void EndCoolDown();
    bool is_cooldown = false; 



};
