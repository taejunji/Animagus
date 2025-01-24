// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * Event Graph에서 처리되는 초기화 및 업데이트 로직을 구현한다.
 * -> 애니메이션에 관한 변수 데이터 초기화 및 업데이트
 * -> 후에 에디터에서 "Anim Graph"를 통해 애니메이션을 결정한다.
 */
UCLASS()
class PROJECT_ANIMAGUS_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    UCharacterAnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    // 한번만 호출
    virtual void NativeInitializeAnimation() override;

    // 틱마다 호출
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    // 일단 플레이어 캐릭터로 -> 나중에 BaseCharacter로 바꿔야할듯
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<class APlayerCharacter> character;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<class UCharacterMovementComponent> movement_component;

protected:
    // 애니메이션을 변경하는 조건을 구할 때 필요한 변수 데이터들 관리
    UPROPERTY(BlueprintReadOnly)
    FVector velocity = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    float ground_speed = 0.f;

    UPROPERTY(BlueprintReadOnly)
    bool b_should_move = false;

    UPROPERTY(BlueprintReadOnly)
    bool b_is_falling = false;

    UPROPERTY(BlueprintReadOnly)
    bool b_dead = false;
};
