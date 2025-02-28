// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
* Animation Instance는 Event Graph, Anim Graph로 이루어져 있다. 
    Event Graph에서 변수의 정보를 통해 Anim Graph에서 상태머신 StateMachine과 
    각종 애니메이션 시스템 ( Montage, Aim Offset, Blend Space ) 등으로 최종 애니메이션을 결정한다.
 * 
 * Event Graph에서 처리되는 초기화 및 업데이트 로직을 구현한다.
 * -> 애니메이션에 관한 변수 데이터 초기화 및 업데이트
 * -> 후에 에디터에서 "Anim Graph"를 통해 애니메이션을 결정한다.
 
    BaseCharacter에서 캐릭터의 상태를 확인해서 
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

    // AnimNotify_(노티파이 이름) : 이 형식을 사용하면 AnimInstance에서 해당 노티파이가 호출될 시점에 함수를 호출해줌
    UFUNCTION()
    void AnimNotify_GetUpEnd();

    UFUNCTION()
    void AnimNotify_StunHardHit();

protected:
    // 일단 플레이어 캐릭터로 -> 나중에 BaseCharacter로 바꿔야할듯
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<class ABaseCharacter> character; 

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<class UCharacterMovementComponent> movement_component;

protected:
    // 애니메이션을 변경하는 조건을 구할 때 필요한 변수 데이터들 관리
    UPROPERTY(BlueprintReadOnly)
    FVector velocity = FVector::ZeroVector; // X,Y,Z 포함한 속도

    UPROPERTY(BlueprintReadOnly)
    float ground_speed = 0.f;  // X,Y만 포함한 지면 속도

    UPROPERTY(BlueprintReadOnly)
    float movement_offset_yaw;  // 캐릭터 이동방향 - 캐릭터가 바라보는 방향 

    UPROPERTY(BlueprintReadOnly)
    bool b_is_moving = false; // 움직이는지

    UPROPERTY(BlueprintReadOnly)
    bool b_is_falling = false; // 떨어지는지

    UPROPERTY(BlueprintReadOnly)
    bool b_is_dead = false; // 죽었는지

    UPROPERTY(BlueprintReadOnly)
    bool b_is_stun = false; 
    // 스턴 상태인지 - 나중에 궁극기 생기면 타이머를 통해 캐릭터를 정지 시킬 것
    // 점프가 아닌 땅인 상태에서 맞으면 스턴에 걸리고 애니메이션이 끝날 동안 "공격, 점프, 움직임" 금지하게 될 것

};
