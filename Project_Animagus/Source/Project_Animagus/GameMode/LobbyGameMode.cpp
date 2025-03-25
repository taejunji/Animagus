// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

ALobbyGameMode::ALobbyGameMode()
{
    // 로비 레벨에서는 Pawn이 필요 없으므로, DefaultPawnClass를 NULL 또는 간단한 Pawn으로 설정
    DefaultPawnClass = nullptr; // 또는 간단한 Pawn 클래스로 지정

    // 로비용 플레이어 컨트롤러를 지정 (이미 프로젝트에 로비용 컨트롤러가 있다면)
    // PlayerControllerClass = ALobbyPlayerController::StaticClass();

    // HUD 위젯을 로드합니다. (예: 경로를 확인하여 할당)
    static ConstructorHelpers::FClassFinder<UUserWidget> LobbyWidgetBP(TEXT("/Game/WorkFolder/Bluprints/UI/WBP_Lobby"));
    if (LobbyWidgetBP.Succeeded())
    {
        LobbyHUDClass = LobbyWidgetBP.Class;
    }
}

