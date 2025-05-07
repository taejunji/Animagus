// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Project_Animagus/GameMode/BattleGameMode.h"


void ALoadingController::BeginPlay()
{
    Super::BeginPlay();

    // 1) 로딩 UI 띄우기 (WBP_Loading을 HUD로 만든 상태라 가정)
    if (LoadingWidgetClass)
    {
        UUserWidget* LoadingUI = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
        if (LoadingUI)
        {
            LoadingUI->AddToViewport();
        }
    }

    // 2) BattleGame 맵 스트리밍 시작
    StartLevelStreaming();
}

void ALoadingController::StartLevelStreaming()
{
   
    FName LevelName = FName("L_Map");

    // 스트리밍 로드 호출
    UGameplayStatics::LoadStreamLevel(
        GetWorld(),
        LevelName,
        /*ShouldBeLoaded=*/ true,
        /*ShouldBeVisible=*/ false,
        /*LatentInfo=*/ FLatentActionInfo()
    );

    // 로드 완료 콜백 바인딩
    // 이 예제는 단순화하여 타이머로 폴링하지만,
    // 실제론 FLatentActionInfo 델리게이트나,
    // Tick()에서 GetStreamingLevel()->IsLevelLoaded() 체크 가능
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
    {
        StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("L_Map"));
        if (StreamingLevel && StreamingLevel->IsLevelLoaded())
        {
            UE_LOG(LogTemp, Error, TEXT("로딩완료"));
            OnLevelLoaded();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("맵 로딩중"));
            // 아직 로드 중이라면 0.1초 뒤에 재시도
            GetWorld()->GetTimerManager().SetTimerForNextTick([this]() { StartLevelStreaming(); });
        }
    });
}

void ALoadingController::OnLevelLoaded()
{
    
    OnAllClientsReady();
    
    // 3) 로드 완료 패킷 전송
    SendPacket_MapLoaded();
}

void ALoadingController::SendPacket_MapLoaded()
{
   // 여기서 완료 패킷 보내기
    
}

void ALoadingController::OnAllClientsReady()
{
    if (!StreamingLevel) return;

    
    StreamingLevel->SetShouldBeVisible(true);
    
    if (LoadingWidgetClass)
    {
        LoadingWidgetClass->Bind();
    }
    
    if (UWorld* World = GetWorld())
    {
        if (ABattleGameMode* GM = Cast<ABattleGameMode>(World->GetAuthGameMode()))
        {
            GM->InitBattleMode();  // StartPlay() 대신 핵심 초기화만 실행
        }
    }
}

