// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "../Character/BaseCharacter.h"

#include "BaseSkill.generated.h"

/**
 *  스킬 기본 베이스 - 공통 작업
 */

UCLASS()
class PROJECT_ANIMAGUS_API ABaseSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseSkill();

    // * 스킬 속성
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString skill_name; // 이름

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    FString skill_description; // 스킬 설명

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Skill")
    //TObjectPtr<class ABaseCharacter> skil_owner; // 주인 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float damage;  // 데미지

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float cool_down_time;  // 쿨타임

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float range;  // 사거리

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
    float life_time;  // 지속시간

public:
    // * 스킬 사용 함수
    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual void ActivateSkill(AActor* Target);  // 스킬 활성화

    UFUNCTION(BlueprintCallable, Category = "Skill")
    virtual void EndSkill();  // 스킬 종료
};
