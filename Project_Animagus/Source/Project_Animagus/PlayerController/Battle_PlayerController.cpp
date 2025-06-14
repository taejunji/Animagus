// Fill out your copyright notice in the Description page of Project Settings.


#include "Battle_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h" // 이동 구현할 때 유틸리티 많은 애 

#include "../Character/PlayerCharacter.h" 
#include "GameFramework/CharacterMovementComponent.h" 
#include "Project_Animagus/Skill/BaseSkill.h"
#include "../UI/MyPlayerHUDWidget.h"
#include "../System/MyGameInstance.h"

#include "../Server/Server/protocol.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"

#include "../GameMode/BattleGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/UI/SkillSelectionWidget.h"

ABattle_PlayerController::ABattle_PlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true; // Tick 활성화

}

void ABattle_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    Skilltest = 0;
    
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
    
    // DisPlayPlayerWidget();
}

void ABattle_PlayerController::DisPlayPlayerWidget()
{
    if (PlayerHUDClass)
    {
        PlayerHUD = CreateWidget<UMyPlayerHUDWidget>(this, PlayerHUDClass);
        if (PlayerHUD)
        {
            PlayerHUD->AddToViewport();
            PlayerHUD->UpdateSelectedSkillOutline(0);
        }

    DisableInput(this); // 입력 비활성화
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

        // "Camera 변경: <-, ->"
        EnhancedInputComponent->BindAction(convert_camera_action, ETriggerEvent::Started, this, &ThisClass::Input_ConvertCamera);

        EnhancedInputComponent->BindAction(init_action, ETriggerEvent::Started, this, &ThisClass::Input_Init);
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
        
        /*FString CurrentSpeedString = FString::Printf(TEXT("Current Speed: %.2f"), MyPlayer->current_speed);
        GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, CurrentSpeedString);
        
        FString CurrentHp = FString::Printf(TEXT("Current Level: %.2d"), MyPlayer->PowerUpLevel);
        GEngine->AddOnScreenDebugMessage( -1, 0.01f, FColor::Green, CurrentHp); */
        
        // 캐릭터의 이동 속도 업데이트
        MyPlayer->SetWalkSpeed(MyPlayer->current_speed);

        // HUD 업데이트: MyPlayerHUDWidget에서 캐릭터의 HP 비율을 업데이트 (hp / max_hp)
        if (PlayerHUD )
        {
            float HPPercent = MyPlayer->GetHP() / MyPlayer->GetMax_Hp();
            PlayerHUD->UpdateHP(HPPercent);
            PlayerHUD->SetCurrentHP(MyPlayer->GetHP(), MyPlayer->GetMax_Hp());

            for (int32 i = 0; i < MyPlayer->Skills.Num(); i++)
            {
                if (MyPlayer->Skills.IsValidIndex(i) && MyPlayer->Skills[i] != nullptr)
                {
                    /*float CooldownPercent = MyPlayer->Skills[i]->GetCooldownPercent();
                    PlayerHUD->UpdateSkillCooldown(i, CooldownPercent);*/

                    int32 CoolDownTime = MyPlayer->Skills[i]->GetRemainingCooldown();
                    PlayerHUD->UpdateSkillCooldownTime(i, CoolDownTime);
                }

                if (MyPlayer->Skills.IsValidIndex(i) && MyPlayer->Skills[i])
                {
                    UTexture2D* Icon = MyPlayer->Skills[i]->SkillIcon;
                    if (PlayerHUD && i< 4)
                    {
                        PlayerHUD->UpdateSkillIcon(i, Icon);
                    }
                }
                
            }
        }


        // Send 판정
        bool ForceSendPacket = false;

        if (LastDesiredInput != DesiredInput)
        {
            ForceSendPacket = true;
            LastDesiredInput = DesiredInput;
        }

        //// State 정보
        //if (DesiredInput == FVector2D::Zero())
        //    MyPlayer->SetMoveState(Protocol::PlayerState::MOVE_STATE_IDLE);
        //else
        //    MyPlayer->SetMoveState(Protocol::PlayerState::MOVE_STATE_RUN);
        // state 를 캐릭터 클래스에서 사용하나?
        // 안하면 그냥 컨트롤러에 박아놓고 사용

        MovePacketSendTimer -= DeltaTime;

        if (MovePacketSendTimer <= 0 || ForceSendPacket)
        {
            // State 설정
            if (MyPlayer->GetMovementComponent()->IsFalling() == false)
            {
                MyPlayer->SetMoveState(Protocol::PlayerState::MOVE_STATE_RUN);
                if (MyPlayer->GetVelocity().Size2D() == 0.0f)
                    MyPlayer->SetMoveState(Protocol::PlayerState::MOVE_STATE_IDLE);
            }

            MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

            Protocol::CS_MOVE_PKT MovePkt;

            // 현재 위치 정보
            {
                FVector Velo = MyPlayer->GetMovementComponent()->Velocity;

                Protocol::PlayerInfo Info;
                FVector Location = MyPlayer->GetActorLocation();
                Info.x = Location.X; Info.y = Location.Y; Info.z = Location.Z;
                Info.rotation = MyPlayer->GetActorRotation().Yaw;
                Info.player_id = MyPlayer->GetPlayerId();
                //Info.player_type = MyPlayer->GetPlayerType();
                Info.player_state = MyPlayer->GetMoveState();
                Info.speed_2d = MyPlayer->GetMovementComponent()->Velocity.Size2D();
                Info.speed_z = MyPlayer->GetMovementComponent()->Velocity.Z;
                //UE_LOG(LogTemp, Warning, TEXT("MySpeed: %f - %d"), Info.speed, MyPlayer->GetPlayerId());

                MovePkt.player_info = Info;
            }

            //UE_LOG(LogTemp, Warning, TEXT("PlayerInfo Send"));

            SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(MovePkt);
            Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
        }

    }
}

void ABattle_PlayerController::Input_Move(const FInputActionValue& InputValue)
{
    FVector2D MoveInput = InputValue.Get<FVector2D>();
    // IMC에 연결된 Action이 행해지면 InputValue에서 값을 추출할 수 있다. 
    // Axis2D의 값 = 2D float => X는 앞뒤 이동, Y는 양옆 이동

    DesiredInput = MoveInput;
    DesiredMoveDirection = FVector::ZeroVector;

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

        DesiredMoveDirection += Direction * MoveInput.Y;
    }

    if (MoveInput.Y != 0)
    {
        FRotator Rotator = GetControlRotation();
        FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0, Rotator.Yaw, 0));
        GetPawn()->AddMovementInput(Direction, MoveInput.Y);

        DesiredMoveDirection += Direction * MoveInput.X;
    }

    DesiredMoveDirection.Normalize();

    auto* MyPlayer = Cast<APlayerCharacter>(GetPawn());
    if (MyPlayer == nullptr) return;

    // 이동 보정 관련
    const FVector Location = MyPlayer->GetActorLocation();
    FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(Location, Location + DesiredMoveDirection);
    DesiredYaw = Rotator.Yaw;


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

        MyPlayer->SetMoveState(Protocol::PlayerState::MOVE_STATE_JUMP);
    }
}

void ABattle_PlayerController::Input_Attack(const FInputActionValue& InputValue)
{
    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Left Mouse Attack!"));

    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        int32 now_skill_idx = MyCharacter->skill_Sellect;
        if (MyCharacter->GetIsDead() == false && MyCharacter->Skills.IsValidIndex(now_skill_idx) && MyCharacter->Skills[now_skill_idx] != nullptr)
        {
            UBaseSkill* Skill = MyCharacter->Skills[now_skill_idx];
            if (Skill->CanActivateSkill() == false) return;

            Skill->ActiveSkill();

            MyCharacter->SetMoveState(Protocol::PlayerState::MOVE_STATE_SKILL);

            // TODO: 유동적으로 스킬 사용 할 수 있도록 설정

            // 스폰 위치: 캐릭터의 전면 (예: 캐릭터 위치에서 전방으로 70cm)
            //FVector Location = MyCharacter->GetActorLocation() + MyCharacter->GetActorForwardVector() * 80.f + MyCharacter->GetActorRightVector() * 30.f;
            //FRotator Rotation11 = MyCharacter->GetController()->GetControlRotation();
            FRotator Rotation = MyCharacter->GetViewRotation();

            //UE_LOG(LogTemp, Warning, TEXT("(%f, %f, %f) - (%f, %f, %f)"), Rotation11.Pitch, Rotation11.Yaw, Rotation11.Roll, Rotation.Pitch, Rotation.Yaw, Rotation.Roll);

            Protocol::CS_USING_SKILL_PKT SkillPkt;
            SkillPkt.player_id = MyCharacter->GetPlayerId();
            SkillPkt.s_type = Skill->SkillType;
            //SkillPkt.x = Location.X; SkillPkt.y = Location.Y; SkillPkt.z = Location.Z;  // 필수인가?
            SkillPkt.pitch = Rotation.Pitch; SkillPkt.yaw = Rotation.Yaw; SkillPkt.roll = Rotation.Roll;

            SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(SkillPkt);
            Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
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
        if (Skilltest == 0){
            MyCharacter->TestSkill_Change();
            Skilltest = 1;
        }
        else if (Skilltest == 1)
        {
            MyCharacter->InitializeSkills();
            Skilltest = 0;
        }
    }
}

void ABattle_PlayerController::Input_ConvertCamera(const FInputActionValue& InputValue)
{
#if 0
    if (APawn* MyPawn = GetPawn()) 
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        // 살아있으면 카메라 변경 못하도록
        if (MyCharacter->GetIsDead() == false)
            return;
    }
#endif

    int32 Direction = FMath::RoundToInt(InputValue.Get<float>()); 
    ABattleGameMode* BM = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

    if (Direction == 0 || BM->IndexingSpawnedPlayers.Num() == 0) return;

    int32 StartIndex = current_camera_index;

    do
    {
        // 방향에 따라 인덱스 조정
        // (현재 인덱스 + 방향 + Num) % Num 구조는 항상 양수 인덱스를 보장하기 위한 패턴
        current_camera_index = (current_camera_index + Direction + BM->IndexingSpawnedPlayers.Num()) % BM->IndexingSpawnedPlayers.Num();

        ABaseCharacter* TargetPawn = BM->IndexingSpawnedPlayers[current_camera_index];
        if (TargetPawn && !TargetPawn->GetIsDead())
        {
            /*PlayerHUD->SetCurrentHP(TargetPawn->GetHP(), TargetPawn->GetMax_Hp());

            float HPPercent = TargetPawn->GetHP() / TargetPawn->GetMax_Hp();
            PlayerHUD->UpdateHP(HPPercent);*/

            SetViewTargetWithBlend(TargetPawn, 0.0f);
            return;
        }

    } while (current_camera_index != StartIndex);
    
}

void ABattle_PlayerController::Input_Skill_1(const FInputActionValue& InputValue)
{
    UE_LOG(LogTemp, Display, TEXT("Skill_1_Pressed"));

    if (APawn* MyPawn  = GetPawn())
    {        
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter->GetIsDead() == false && MyCharacter->Skills.IsValidIndex(0) && MyCharacter->Skills[0])
        {
            MyCharacter->skill_Sellect = 0;
        }
        if (PlayerHUD)
        {
            PlayerHUD->UpdateSelectedSkillOutline(0);
        }
    }
}

void ABattle_PlayerController::Input_Skill_2(const FInputActionValue& InputValue)
{
    UE_LOG(LogTemp, Display, TEXT("Skill_2_Pressed"));

    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter->GetIsDead() == false && MyCharacter->Skills.IsValidIndex(1) && MyCharacter->Skills[1])
        {
            MyCharacter->skill_Sellect = 1;
        }

        if (PlayerHUD)
        {
            PlayerHUD->UpdateSelectedSkillOutline(1);
        }
    }

}

void ABattle_PlayerController::Input_Skill_3(const FInputActionValue& InputValue)
{
    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter->GetIsDead() == false && MyCharacter->Skills.IsValidIndex(2) && MyCharacter->Skills[2])
        {
            MyCharacter->skill_Sellect = 2;
        }

        if (PlayerHUD)
        {
            PlayerHUD->UpdateSelectedSkillOutline(2);
        }
    }
}

void ABattle_PlayerController::Input_Skill_4(const FInputActionValue& InputValue)
{
    if (APawn* MyPawn  = GetPawn())
    {
        ABaseCharacter* MyCharacter = Cast<ABaseCharacter>(MyPawn);
        
        if (MyCharacter->GetIsDead() == false && MyCharacter->Skills.IsValidIndex(3) && MyCharacter->Skills[3])
        {
            MyCharacter->skill_Sellect = 3;
        }

        if (PlayerHUD)
        {
            PlayerHUD->UpdateSelectedSkillOutline(3);
        }
    }
    UE_LOG(LogTemp, Display, TEXT("Skill_4_Pressed"));
}

void ABattle_PlayerController::Input_Init(const FInputActionValue& InputValue)
{
    UE_LOG(LogTemp, Warning, TEXT("초기화 입력 요청"));

    ABattleGameMode* BattleMode = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (BattleMode != nullptr && BattleMode->AmIHost == true)
    {
        Protocol::CS_TIME_OVER_PKT timeOverPkt;

        SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(timeOverPkt);
        Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
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

USkillSelectionWidget* ABattle_PlayerController::ShowSkillSelectionWidget(float TimeLimit)
{
    if (!SkillSelectionWidgetClass) return nullptr;

    // 1) 게임 모드 전환: UI 전용
    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(nullptr);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);
    bShowMouseCursor = true;

    // 2) 위젯 생성 및 세팅
    SkillSelectionWidget = CreateWidget<USkillSelectionWidget>(this, SkillSelectionWidgetClass);
    SkillSelectionWidget->AddToViewport();
    SkillSelectionWidget->SetupOwner(this);
    SkillSelectionWidget->SetupWidget(TimeLimit);

    return SkillSelectionWidget;
}

void ABattle_PlayerController::OnSkillSelectionConfirmed(const TArray<TSubclassOf<UBaseSkill>>& Selected)
{
    if (ABaseCharacter* Ch = Cast<APlayerCharacter>(GetPawn()))
    {
        Ch->SelectSkills(Selected);
    }

    // 2) 입력 모드 복구: 게임플레이 모드
    FInputModeGameOnly GameMode;
    SetInputMode(GameMode);
    bShowMouseCursor = false;

    // 3) 5초 카운트다운 시작 (GameMode 함수)
    if (ABattleGameMode* GM = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode()))
    {
       // GM->BeginRoundCountdown();
    }
    
}


