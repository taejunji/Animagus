// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPG_PlayerController.generated.h"

/**
 * 
 */
struct FInputActionValue;

UCLASS()
class PROJECT_ANIMAGUS_API ARPG_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    ARPG_PlayerController(const FObjectInitializer& ObjectInitializer);
     
protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void Input_Move(const FInputActionValue& InputValue);
    void Input_Turn(const FInputActionValue& InputValue);

protected:

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputMappingContext> InputMappingContext;
    
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> TurnAction;
};
