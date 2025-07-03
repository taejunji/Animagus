// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Item/BaseItem.h"
#include "../UI/SkillSelectionWidget.h"
#include "GameFramework/PlayerController.h"
#include "Battle_PlayerController.generated.h"


class URoundResultWidget;

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
    int32 current_camera_index = 0;

public:
    ABattle_PlayerController(const FObjectInitializer& ObjectInitializer);

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void Input_Move(const FInputActionValue& InputValue);
    void Input_Rotate(const FInputActionValue& InputValue);
    void Input_Jump(const FInputActionValue& InputValue);
    void Input_Attack(const FInputActionValue& InputValue);
    void Input_Ready(const FInputActionValue& InputValue);
    
    void Input_ConvertCamera(const FInputActionValue& InputValue);

    void Input_Skill_1(const FInputActionValue& InputValue);
    void Input_Skill_2(const FInputActionValue& InputValue);
    void Input_Skill_3(const FInputActionValue& InputValue);
    void Input_Skill_4(const FInputActionValue& InputValue);

   // void Input_Skill1(const FInputActionValue& InputValue);
   // void Input_Skill2(const FInputActionValue& InputValue);
   // void Input_Skill3(const FInputActionValue& InputValue);
   // void Input_Skill4(const FInputActionValue& InputValue);


    
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
    TObjectPtr<class UInputAction> convert_camera_action;


    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> control_toggle_action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> run_toggle_action;


    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> skill_1_action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> skill_2_action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> skill_3_action;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<class UInputAction> skill_4_action;

public:
    // HUD 위젯 클래스를 에디터에서 지정할 수 있도록 함
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UMyPlayerHUDWidget> PlayerHUDClass;

    UPROPERTY(EditDefaultsOnly, Category="UI")
    TSubclassOf<USkillSelectionWidget> SkillSelectionWidgetClass;
    
    // 생성된 HUD 위젯 인스턴스
    UPROPERTY()
    class UMyPlayerHUDWidget* PlayerHUD;

    UPROPERTY()
    class USkillSelectionWidget* SkillSelectionWidget;
    
    UFUNCTION()
    USkillSelectionWidget* ShowSkillSelectionWidget(float TimeLimit);
    
    UFUNCTION()
    void DisPlayPlayerWidget();
    
    UFUNCTION()// 위젯에서 호출될 스킬 확정 핸들러
    void OnSkillSelectionConfirmed(const TArray<TSubclassOf<UBaseSkill>>& Selected);
    
    UPROPERTY()
    int32 Skilltest;

public:
    
    // 감도 턴측
    void TurnAtRate(float Rate);
    // 감도 룩측
    void LookUpAtRate(float Rate);

protected:
   // 감도 정도 
    float CachedMouseSensitivity = 1.0f;

public:
    
    UFUNCTION()
    void ShowRoundResults(
        const TArray<FString>& IDs,
        const TArray<int32>& Scores,
        float DisplayTime
    );
    
    void ShowRoundResults(
        const TArray<FString>& IDs,
        const TArray<int32>& Scores
    );
    
    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<URoundResultWidget> RoundResultWidgetClass;

    UPROPERTY(EditAnywhere, Category="UI")
    URoundResultWidget* RoundResultwidget; 
};


