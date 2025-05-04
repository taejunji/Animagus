// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

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
    if (LobbyWidgetClass)
    {
        LobbyWidget = CreateWidget<UUserWidget>(this, LobbyWidgetClass);
        UE_LOG(LogTemp, Log, TEXT("LobbyWidget 생성: %s"), LobbyWidget ? TEXT("성공") : TEXT("실패"));

        if (LobbyWidget)
        {
            LobbyWidget->AddToViewport();

            // StartButton 위젯 찾아서 바인딩
            Start_Button = Cast<UButton>(LobbyWidget->GetWidgetFromName(TEXT("Start_Button")));
            UE_LOG(LogTemp, Log, TEXT("StartButton 찾기: %s"), Start_Button ? TEXT("성공") : TEXT("실패"));
            if (Start_Button)
            {
                // 클릭 이벤트에 OnStartButtonClicked 연결
                Start_Button->OnClicked.AddDynamic(this, &ALobbyPlayerController::OnStartButtonClicked);

                // 처음에는 비활성화
                Start_Button->SetIsEnabled(false);

                // 5초 뒤에 EnableStartButton 호출
                GetWorld()->GetTimerManager().SetTimer(
                    StartButtonTimerHandle,
                    this,
                    &ALobbyPlayerController::EnableStartButton,
                    5.0f,
                    false
                );
                UE_LOG(LogTemp, Log, TEXT("EnableStartButton 타이머 설정 완료"));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LobbyWidgetClass가 바인딩되지 않음"));
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

void ALobbyPlayerController::OnStartButtonClicked()
{
    // 버튼 클릭 시 L_Map 레벨로 전환
    static const FName TargetLevelName = TEXT("L_Map");
    UE_LOG(LogTemp, Log, TEXT("Start 버튼 클릭! 레벨 '%s' 로 전환합니다."), *TargetLevelName.ToString());
    
    // OpenLevel 호출:
    UGameplayStatics::OpenLevel(this, TargetLevelName);
}

void ALobbyPlayerController::EnableStartButton()
{
    if (Start_Button)
    {
        Start_Button->SetIsEnabled(true);
    } 
}
