// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h" // 이동 구현할 때 유틸리티 많은 애 

#include "../Character/PlayerCharacter.h" 

ABattle_PlayerController::ABattle_PlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true; // Tick 활성화

}

void ABattle_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    if(auto* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // IMC 등록 및 Priority는 0으로
        SubSystem->AddMappingContext(input_mapping_context, 0);

        // 입력 매핑 컨텍스트 해제하는 법
        // 1. SubSystem->RemoveMappingContext(input_mapping_context);
        // 2. SubSystem->ClearAllMappings();
    }

    if (auto* MyPlayer = Cast<APlayerCharacter>(GetPawn())) 
    {
        // TPS 설정을 위한 변수 설정 [ 컨트롤러 회전 Yaw 끄기, Spring Arm 폰 제어 회전 켜기, Character Movement 컨트롤러 선호 회전 켜기 ]
        MyPlayer->Initialize_TPS_Settings(); 
    }
}

void ABattle_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(move_action, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
        EnhancedInputComponent->BindAction(rotate_action, ETriggerEvent::Triggered, this, &ThisClass::Input_Rotate);
        EnhancedInputComponent->BindAction(jump_action, ETriggerEvent::Triggered, this, &ThisClass::Input_Jump);

        EnhancedInputComponent->BindAction(control_toggle_action, ETriggerEvent::Started, this, &ThisClass::Input_ControlToggle_Pressed);
        EnhancedInputComponent->BindAction(control_toggle_action, ETriggerEvent::Completed, this, &ThisClass::Input_ControlToggle_Released);

        EnhancedInputComponent->BindAction(run_toggle_action, ETriggerEvent::Started, this, &ThisClass::Input_RunToggle_Pressed);
        EnhancedInputComponent->BindAction(run_toggle_action, ETriggerEvent::Completed, this, &ThisClass::Input_RunToggle_Released);
    }
}

void ABattle_PlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); 

    if (auto* MyPlayer = Cast<APlayerCharacter>(GetPawn())) 
    {
        // 목표 속도 설정 (달리기 상태에 따라 달라짐)
        float TargetSpeed = is_running ? MyPlayer->default_run_speed : MyPlayer->default_walk_speed; 

        // 현재 속도를 목표 속도로 점진적으로 변경 
		// 내부적으로 DeltaTime * speed_change_rete라서 1초에 5.f의 속도가 변하길 기대했는데 디버깅 해보니 이론과 다름
        MyPlayer->current_speed = FMath::FInterpTo(MyPlayer->current_speed, TargetSpeed, DeltaTime, MyPlayer->speed_change_rete);  

        FString CurrentSpeedString = FString::Printf(TEXT("Current Speed: %.2f"), MyPlayer->current_speed);
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, CurrentSpeedString);

        // 캐릭터의 이동 속도 업데이트
        MyPlayer->SetWalkSpeed(MyPlayer->current_speed); 
    }
}

void ABattle_PlayerController::Input_Move(const FInputActionValue& InputValue)
{
    FVector2D MoveInput = InputValue.Get<FVector2D>();
    // IMC에 연결된 Action이 행해지면 InputValue에서 값을 추출할 수 있다. 
    // Axis2D의 값 = 2D float => X는 앞뒤 이동, Y는 양옆 이동

    if (MoveInput.X != 0)
    {
        // 플레이어 컨트롤러가 바라보는 방향에 따라 캐릭터가 이동할 방향을 계산하고 움직임을 처리하는 코드

        // 1. 현재 컨트롤러가 바라보는 방향의 회전 정보 
        // 2. UKismetMathLibrary::GetForwardVector 함수는 입력받은 FRotator를 기준으로 
        //      기본 전방 Forward 벡터 (1, 0, 0)을 회전시켜 새로운 방향 벡터를 반환
        // 
        //      FRotator(0, Rotator.Yaw, 0): 컨트롤러의 전체 회전값 중 Yaw(좌우 회전) 값만 사용하고, Pitch와 Roll은 0으로 설정 =  수평면에서 바라보는 방향만 반영
        //
        // 3. 지정된 방향(Direction)으로 플레이어의 이동을 처리, MoveInput.X는 1 또는 -1
        FRotator Rotator = GetControlRotation();
        FVector Direction = UKismetMathLibrary::GetForwardVector(FRotator(0, Rotator.Yaw, 0));
        GetPawn()->AddMovementInput(Direction, MoveInput.X);
    }

    if (MoveInput.Y != 0)
    {
        FRotator Rotator = GetControlRotation();
        FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0, Rotator.Yaw, 0));
        GetPawn()->AddMovementInput(Direction, MoveInput.Y);
    }
}

void ABattle_PlayerController::Input_Rotate(const FInputActionValue& InputValue)
{
    FVector2D MouseInput = InputValue.Get<FVector2D>();
    
    AddYawInput(MouseInput.X);
    AddPitchInput(MouseInput.Y);
}

void ABattle_PlayerController::Input_Jump(const FInputActionValue& InputValue)
{
    if (auto* MyPlayer = Cast<APlayerCharacter>(GetPawn()))
    {
        MyPlayer->Jump();
    }
}

void ABattle_PlayerController::Input_ControlToggle_Pressed()
{
    // 키가 눌렸을 때 -> RPG 

    if (control_type == ControllerType::RPG) {
        return;
    }
    else {
        control_type = ControllerType::RPG;
        if (auto* MyPlayer = Cast<APlayerCharacter>(GetPawn()))
        {
            MyPlayer->Initialize_RPG_Settings();
        }
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("RPG 컨트롤 모드"));
    }
}

void ABattle_PlayerController::Input_ControlToggle_Released()
{
    // 카가 떼진 상태 -> TPG 유지

    if (control_type == ControllerType::TPS) {
        return;
    }
    else {
        control_type = ControllerType::TPS;
        if (auto* MyPlayer = Cast<APlayerCharacter>(GetPawn()))
        {
            MyPlayer->Initialize_TPS_Settings();
        }
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("TPS 컨트롤 모드"));
}

void ABattle_PlayerController::Input_RunToggle_Pressed()
{
    is_running = true;
}

void ABattle_PlayerController::Input_RunToggle_Released()
{
    is_running = false;
}


