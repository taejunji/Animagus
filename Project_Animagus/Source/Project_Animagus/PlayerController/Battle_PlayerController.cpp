// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h" // 이동 구현할 때 유틸리티 많은 애 

#include "../Character/PlayerCharacter.h" 
#include "GameFramework/CharacterMovementComponent.h" 
#include "Project_Animagus/Skill/BaseSkill.h"
#include "../UI/MyPlayerHUDWidget.h"

ABattle_PlayerController::ABattle_PlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true; // Tick 활성화

}

void ABattle_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 마우스 커서를 숨기고 UI에서 잠금
    bShowMouseCursor = false; 
    SetInputMode(FInputModeGameOnly()); // 게임 입력 전용 모드 -> 죽으면 UI만 입력 가능하도록

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

    if (PlayerHUDClass)
    {
        PlayerHUD = CreateWidget<UMyPlayerHUDWidget>(this, PlayerHUDClass);
        if (PlayerHUD)
        {
            PlayerHUD->AddToViewport();
        }
    }
}

void ABattle_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Triggered : 입력 키를 누르고 있는 동안 지속적으로 발생
    // Started : 키를 누르는 순간 단 한 번 발생
    // Completed : 키를 놓는 순간 한 번 발생
    if (auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // "W,A,S,D", "Mouse", "Space"
        EnhancedInputComponent->BindAction(move_action, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
        EnhancedInputComponent->BindAction(rotate_action, ETriggerEvent::Triggered, this, &ThisClass::Input_Rotate);
        EnhancedInputComponent->BindAction(jump_action, ETriggerEvent::Triggered, this, &ThisClass::Input_Jump);

        // "Mosue Left/Right Click"
        EnhancedInputComponent->BindAction(attack_action, ETriggerEvent::Started, this, &ThisClass::Input_Attack); 
        EnhancedInputComponent->BindAction(ready_action, ETriggerEvent::Started, this, &ThisClass::Input_Ready);  

        // "V"
        EnhancedInputComponent->BindAction(control_toggle_action, ETriggerEvent::Started, this, &ThisClass::Input_ControlToggle_Pressed); 
        EnhancedInputComponent->BindAction(control_toggle_action, ETriggerEvent::Completed, this, &ThisClass::Input_ControlToggle_Released);
        // "Shift"
        EnhancedInputComponent->BindAction(run_toggle_action, ETriggerEvent::Started, this, &ThisClass::Input_RunToggle_Pressed);
        EnhancedInputComponent->BindAction(run_toggle_action, ETriggerEvent::Completed, this, &ThisClass::Input_RunToggle_Released);

        // "Skill - 1,2,3,4"
        EnhancedInputComponent->BindAction(skill_1_action, ETriggerEvent::Started, this, &ThisClass::Input_Skill_1);
        EnhancedInputComponent->BindAction(skill_2_action, ETriggerEvent::Started, this, &ThisClass::Input_Skill_2);
        EnhancedInputComponent->BindAction(skill_3_action, ETriggerEvent::Started, this, &ThisClass::Input_Skill_3);
        EnhancedInputComponent->BindAction(skill_4_action, ETriggerEvent::Started, this, &ThisClass::Input_Skill_4);
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
        MyPlayer->current_speed = FMath::FInterpTo(MyPlayer->current_speed, TargetSpeed, DeltaTime, MyPlayer->speed_change_rate);
        
        FString CurrentSpeedString = FString::Printf(TEXT("Current Speed: %.2f"), MyPlayer->current_speed);
        GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, CurrentSpeedString);
        
        FString CurrentHp = FString::Printf(TEXT("Current Hp: %.2f"), MyPlayer->GetHP());
        GEngine->AddOnScreenDebugMessage( -1, 0.01f, FColor::Green, CurrentHp); 
        
        // 캐릭터의 이동 속도 업데이트
        MyPlayer->SetWalkSpeed(MyPlayer->current_speed);

        // HUD 업데이트: MyPlayerHUDWidget에서 캐릭터의 HP 비율을 업데이트 (hp / max_hp)
        if (PlayerHUD)
        {
            float HPPercent = MyPlayer->GetHP() / MyPlayer->GetMax_Hp();
            PlayerHUD->UpdateHP(HPPercent);
            
            for (int32 i = 0; i < MyPlayer->Skills.Num(); i++)
            {
                if (MyPlayer->Skills.IsValidIndex(i) && MyPlayer->Skills[i] != nullptr)
                {
                    float CooldownPercent = MyPlayer->Skills[i]->GetCooldownPercent();
                    PlayerHUD->UpdateSkillCooldown(i, CooldownPercent);
                }
            }
        }
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

void ABattle_PlayerController::Input_Attack(const FInputActionValue& InputValue)
{
    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Left Mouse Attack!"));

    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter && MyCharacter->Skills.IsValidIndex(0) && MyCharacter->Skills[0])
        {
            MyCharacter->Skills[0]->ActiveSkill();
        }
    }

    // auto* MyPlayer = Cast<ABaseCharacter>(GetPawn());
    //
    // if (MyPlayer) MyPlayer->PlayAnimMontageByType(MontageType::DefaultAttack);
    //
    // if (MyPlayer && MyPlayer->Skills.IsValidIndex(0) && MyPlayer->Skills[0])
    // {
    //     MyPlayer->Skills[0]->ActiveSkill();
    // }
}

void ABattle_PlayerController::Input_Ready(const FInputActionValue& InputValue)
{
    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Right Mouse Attack!"));

    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter && MyCharacter->Skills.IsValidIndex(1) && MyCharacter->Skills[1])
        {
            MyCharacter->Skills[1]->ActiveSkill();
        }
    } 
}

void ABattle_PlayerController::Input_Skill_1(const FInputActionValue& InputValue)
{
    UE_LOG(LogTemp, Display, TEXT("Skill_1_Pressed"));

    if (APawn* MyPawn  = GetPawn())
    {        
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter && MyCharacter->Skills.IsValidIndex(4) && MyCharacter->Skills[4])
        {
            MyCharacter->Skills[4]->ActiveSkill();
        }
    }
}

void ABattle_PlayerController::Input_Skill_2(const FInputActionValue& InputValue)
{
    UE_LOG(LogTemp, Display, TEXT("Skill_2_Pressed"));

    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter && MyCharacter->Skills.IsValidIndex(1) && MyCharacter->Skills[1])
        {
            MyCharacter->Skills[1]->ActiveSkill();
        }
    }

}

void ABattle_PlayerController::Input_Skill_3(const FInputActionValue& InputValue)
{
    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter && MyCharacter->Skills.IsValidIndex(2) && MyCharacter->Skills[2])
        {
            MyCharacter->Skills[2]->ActiveSkill();
        }
    }
}

void ABattle_PlayerController::Input_Skill_4(const FInputActionValue& InputValue)
{
    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter && MyCharacter->Skills.IsValidIndex(3) && MyCharacter->Skills[3])
        {
            MyCharacter->Skills[3]->ActiveSkill();
        }
    }
    UE_LOG(LogTemp, Display, TEXT("Skill_4_Pressed"));
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


