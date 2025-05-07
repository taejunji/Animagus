// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LevelStreamingDynamic.h"
#include "LoadingController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API ALoadingController : public APlayerController
{
	GENERATED_BODY()
  

public:
    // 위젯 클래스 에디터에서 지정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
    TSubclassOf<UUserWidget> LoadingWidgetClass;

public:
    // 서버에 "맵 로드 완료" 패킷 전송 (구현은 주석 처리)
    void SendPacket_MapLoaded();

    // 서버로부터 "모두 준비 완료" 패킷을 받았을 때 호출
    UFUNCTION()
    void OnAllClientsReady();

protected:
    virtual void BeginPlay() override;

    // BattleGame 레벨 스트리밍 로드 요청
    void StartLevelStreaming();

    // 맵 로드 완료 후 호출되는 콜백
    UFUNCTION()
    void OnLevelLoaded();

    // 스트리밍 레벨 핸들
    ULevelStreaming* StreamingLevel = nullptr;
};
