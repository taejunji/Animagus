// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPS_PlayerController.generated.h"

struct FInputActionValue;

/**
 * 
 */
//------------------------------------------------------------------------------
UCLASS()
class PROJECT_ANIMAGUS_API ATPS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    ATPS_PlayerController(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void Input_Move(const FInputActionValue& InputValue);
    void Input_Rotate(const FInputActionValue& InputValue);
    void Input_Jump(const FInputActionValue& InputValue);

protected:

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputMappingContext> input_mapping_context;

    // 이름만 Action이고 설정된 타입에 따라 Axis든 Action이든 데이터를 반환한다.
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> move_action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> rotate_action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> jump_action;

};
