// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "../Character/BaseCharacter.h"

#include "BaseSkill.generated.h"

/**
 * 스킬 기본 베이스 - 공통 작업
 */
<<<<<<< HEAD
UCLASS(Blueprintable)
class PROJECT_ANIMAGUS_API UBaseSkill : public UObject
{
    GENERATED_BODY()

public:
    // 스킬 이름
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Skill")
    FString SkillName;

    // 스킬 설명
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Skill")
    FString SkillDescription;

    // 스킬 소유자. 실제 캐릭터 클래스(예: ABaseCharacter)를 사용합니다.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Skill")
    TObjectPtr<class ABaseCharacter> Owner;

    // 스킬 쿨타임 시간(초)
    UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Skill")
    float CooldownTime;

    // 현재 쿨다운 상태
    UPROPERTY(BlueprintReadOnly, Category = "Skill")
    bool bIsCooldown;

    // 마지막 사용 시각 (초)
    UPROPERTY(BlueprintReadOnly, Category = "Skill")
    float LastUsedTime;

public:
    UBaseSkill();

    // 스킬 활성화 (실제로 스킬 효과를 실행할 때 호출)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
    void ActiveSkill();
    virtual void ActiveSkill_Implementation();

    // 스킬 비활성화 (예: 지속 효과 종료)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
    void DeactiveSkill();
    virtual void DeactiveSkill_Implementation();

    // 쿨다운 시작 (스킬 사용 후 호출)
    UFUNCTION(BlueprintCallable, Category = "Skill")
    void StartCooldown();

    // 쿨다운 종료 (쿨타임이 만료된 후 호출)
    UFUNCTION(BlueprintCallable, Category = "Skill")
    void EndCooldown();

    // 현재 쿨다운 중인지 확인하는 함수
    UFUNCTION(BlueprintCallable, Category = "Skill")
    bool IsOnCooldown() const;
=======

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
>>>>>>> main
};
