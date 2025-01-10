// Fill out your copyright notice in the Description page of Project Settings.


#include "RPG_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


ARPG_PlayerController::ARPG_PlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

void ARPG_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem)
    {
        Subsystem->AddMappingContext(InputMappingContext, 0);
    }
}

void ARPG_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
        EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Turn);
    }

}

void ARPG_PlayerController::Input_Move(const FInputActionValue& InputValue)
{
    FVector2D MoveVector = InputValue.Get<FVector2D>();

}

void ARPG_PlayerController::Input_Turn(const FInputActionValue& InputValue)
{
    float x_value = InputValue.Get<float>();
    float y_value = InputValue.Get<float>();
}
