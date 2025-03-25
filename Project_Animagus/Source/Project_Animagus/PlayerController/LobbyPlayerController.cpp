// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void ALobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    // 마우스 커서를 보이게 하고, UI 전용 입력 모드로 전환합니다.
    bShowMouseCursor = true;
    FInputModeUIOnly InputMode;
    // 특정 위젯에 포커스를 주고 싶으면 InputMode.SetWidgetToFocus(WidgetRef)를 사용합니다.
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(InputMode);

    // 로비 HUD 위젯 생성 및 Viewport에 추가합니다.
    if (LobbyHUDClass)
    {
        LobbyHUD = CreateWidget<UUserWidget>(this, LobbyHUDClass);
        if (LobbyHUD)
        {
            LobbyHUD->AddToViewport();
        }
    }
}

void ALobbyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    // 로비 레벨에서는 별도의 게임 내 Pawn 제어는 필요 없으므로,
    // 기본적으로 UI 입력만 처리합니다.
}

void ALobbyPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 로비 레벨에서는 UI 상호작용(예: 버튼 클릭, 커서 움직임)만 처리하면 됩니다.
    // 별도의 게임 내 이동이나 액션 입력은 필요하지 않으므로 Tick()에서 특별한 처리는 하지 않습니다.
}

