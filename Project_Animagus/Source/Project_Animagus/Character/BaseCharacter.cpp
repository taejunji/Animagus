// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

    // 점프 속도를 500으로 설정
    // 기본 걷기 속도 300으로 설정 - 1초에 3m
    GetCharacterMovement()->JumpZVelocity = 500.0f;

    default_walk_speed = 300.f;
    default_run_speed = 500.f;
    speed_change_rete = 5.f; // 1초에 5.f정도의 속도 변화를 꿈 꿨는데 뭔가 이상하다 
    current_speed = default_walk_speed;

    SetWalkSpeed(default_walk_speed);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWalkSpeed(float fValue)
{
    GetCharacterMovement()->MaxWalkSpeed = fValue;
}



