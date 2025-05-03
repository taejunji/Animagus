// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "../PlayerController/LobbyPlayerController.h"
#include "Kismet/GameplayStatics.h"

#include "../System//MyGameInstance.h"
#include "../Network/ClientPacketHandler.h"


ALobbyGameMode::ALobbyGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // 로비 레벨에서는 Pawn이 필요 없으므로, DefaultPawnClass를 NULL 또는 간단한 Pawn으로 설정
    DefaultPawnClass = nullptr; // 또는 간단한 Pawn 클래스로 지정

    // 로비용 플레이어 컨트롤러를 지정 (이미 프로젝트에 로비용 컨트롤러가 있다면)
    // PlayerControllerClass = ALobbyPlayerController::StaticClass();

    UE_LOG(LogTemp, Warning, TEXT("LobbyMode 생성자"));

}

void ALobbyGameMode::StartPlay()
{
    Super::StartPlay();

}

void ALobbyGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //Cast<UMyGameInstance>(GWorld->GetGameInstance())->HandleRecvPackets();
}

void ALobbyGameMode::ActiveStartButton()
{
    //UE_LOG(LogTemp, Warning, TEXT("ActiveStartButton 시작"));

    //UWorld* World = GetWorld();
    //if (!World)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode: World가 null임."));
    //    return;
    //}

    //ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(UGameplayStatics::GetPlayerController(World, 0));
    //if (PC == nullptr)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode: World가 null임."));
    //    return;
    //}

    //PC->ActiveStartButton();
}


