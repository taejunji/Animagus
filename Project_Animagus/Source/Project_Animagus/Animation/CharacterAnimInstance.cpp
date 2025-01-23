// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/PlayerCharacter.h"

UCharacterAnimInstance::UCharacterAnimInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    character = Cast< APlayerCharacter>(TryGetPawnOwner());
    if (character) movement_component = character->GetCharacterMovement(); 
    
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (character == nullptr) return;
    if (movement_component == nullptr) return;

    velocity = movement_component->Velocity;
    ground_speed = velocity.Size2D(); // X,Y축 평지 속도만 추출

    // **3.f**는 단순히 속도의 임계값으로, 이 값 이상일 경우 움직이고 있다고 판단하는 것입니다. 
    // 속도가 3보다 작으면 움직이지 않는 것으로 간주 + 캐릭터가 현재 가속 중인지 확인
    b_should_move = (ground_speed > 3.f && movement_component->GetCurrentAcceleration() != FVector::ZeroVector);
    b_is_falling = movement_component->IsFalling(); 
} 
