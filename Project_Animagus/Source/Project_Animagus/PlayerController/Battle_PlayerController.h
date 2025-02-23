// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Battle_PlayerController.generated.h"

struct FInputActionValue;

enum class ControllerType { TPS, RPG };
/**
 * 
 */
//------------------------------------------------------------------------------
UCLASS()
class PROJECT_ANIMAGUS_API ABattle_PlayerController : public APlayerController
{
	GENERATED_BODY()
    
private:
    ControllerType control_type = ControllerType::TPS; // 기본이 TPS, Alt 누르면 RPG
    bool is_running = false;

public:
    ABattle_PlayerController(const FObjectInitializer& ObjectInitializer);

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // * 스킬 입력
    void FireProjectileFromPlayer();

private:
    void Input_Move(const FInputActionValue& InputValue);
    void Input_Rotate(const FInputActionValue& InputValue);
    void Input_Jump(const FInputActionValue& InputValue);
    void Input_Attack(const FInputActionValue& InputValue);
    void Input_Ready(const FInputActionValue& InputValue);

    void Input_ControlToggle_Pressed();     // 키가 눌렸을 때 -> 잠시동안 RPG 모드
    void Input_ControlToggle_Released();    // 키가 떼질 때   -> 잠시동안 TPS 모드

    void Input_RunToggle_Pressed();  // 달리기
    void Input_RunToggle_Released(); // 걷기

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

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> attack_action;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> ready_action;


    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> control_toggle_action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> run_toggle_action;
};
