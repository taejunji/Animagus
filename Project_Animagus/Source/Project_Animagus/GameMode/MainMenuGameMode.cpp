// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "../PlayerController/MainMenuController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
    // 이 GameMode의 기본 PlayerController를 메인 메뉴 컨트롤러로 지정
    PlayerControllerClass = AMainMenuController::StaticClass();

    // 메뉴에서는 Pawn이 필요 없으므로 None 설정
    DefaultPawnClass = nullptr;
}

void AMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
    // 추가 초기화가 필요 없으면 비워둡니다.
}

