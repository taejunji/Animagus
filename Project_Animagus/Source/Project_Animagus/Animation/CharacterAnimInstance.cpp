// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Character/BaseCharacter.h"

UCharacterAnimInstance::UCharacterAnimInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    character = Cast<ABaseCharacter>(TryGetPawnOwner());
    if (character) movement_component = character->GetCharacterMovement(); 
    
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (character == nullptr || movement_component == nullptr) return;

    b_is_dead = character->GetIsDead();
    if (b_is_dead) return;


    velocity = movement_component->Velocity; // X,Y,Z 속도
    ground_speed = velocity.Size2D(); // X,Y축 평지 속도만 추출

    // ---- 미완성 ----- 에임 오프셋 제작중인데 안쓸 수도
    FRotator AimRotation = character->GetActorRotation(); // 캐릭터가 바라보는 방향 
    FRotator DeltaRotation = FRotator::ZeroRotator; 
    if (!velocity.IsNearlyZero()) // 속도가 거의 0이 아니면 각도 계산 
    {
        FRotator MoveRotation = UKismetMathLibrary::MakeRotFromX(velocity); // 캐릭터 이동 방향 
        DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(MoveRotation, AimRotation); // 각도 차이 
    }
    movement_offset_yaw = DeltaRotation.Yaw; // 이 값으로 Blend Space의 좌,우,앞,뒤 애니메이션 조절
    // ---- 미완성 -----

    // **3.f**는 단순히 속도의 임계값으로, 이 값 이상일 경우 움직이고 있다고 판단하는 것입니다. 
    // 속도가 3보다 작으면 움직이지 않는 것으로 간주 + 캐릭터가 현재 가속 중인지 확인
    b_is_moving = (ground_speed > 3.f && movement_component->GetCurrentAcceleration() != FVector::ZeroVector);
    b_is_falling = movement_component->IsFalling(); 
}

void UCharacterAnimInstance::AnimNotify_GetUpEnd()
{
    // getting_up_cut 애니메이션에 Notify(노티파이)를 설정해서 애니매이션이 종료할 때 다시 움직일 수 있도록 이벤트 함수 설정

    character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}


