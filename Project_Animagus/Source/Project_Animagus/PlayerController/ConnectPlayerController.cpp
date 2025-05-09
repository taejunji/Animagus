// Fill out your copyright notice in the Description page of Project Settings.


#include "ConnectPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../UI/UConnectWidget.h"         // UConnectWidget 선언 포함
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "../System/MyGameInstance.h"


void AConnectPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 1) 위젯 클래스가 할당되었는지 확인
    if (!ConnectWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ConnectWidgetClass가 에디터에서 할당되지 않음"));
        return;
    }

    // 2) 위젯 생성 후 뷰포트에 추가
    ConnectWidget = CreateWidget<UUConnectWidget>(this, ConnectWidgetClass);
    if (ConnectWidget)
    {
        ConnectWidget->AddToViewport();

        // 3) 위젯 내부의 버튼에 클릭 델리게이트 바인딩
        ConnectWidget->ConnectButton->OnClicked.AddDynamic(this, &AConnectPlayerController::OnConnectButtonClicked);

        // UI 전용 입력 모드
        bShowMouseCursor = true;
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(ConnectWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(InputMode);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ConnectWidget 생성 실패"));
    }
}

void AConnectPlayerController::OnConnectButtonClicked()
{
    if (!ConnectWidget) return;

    // 4) 입력된 IP 주소 읽어오기
    const FString TargetIP = ConnectWidget->InputIP->GetText().ToString();
    if (TargetIP.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("IP 주소가 비어있음"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("입력된 IP: %s 로 연결 시도"), *TargetIP);

    
    // 여기서 Connect 하면됩니다잉. 
    Cast<UMyGameInstance>(GWorld->GetGameInstance())->ConnectToGameServer(TargetIP);

    // 6) 연결 성공 시, 로비 레벨로 전환
    UGameplayStatics::OpenLevel(this, TEXT("Lobby"));
}