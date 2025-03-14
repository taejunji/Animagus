// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

/**
 *  한 레벨 당 하나의 게임모드를 설정할 수 있으며 게임 모드는 DefaultPawn(디폴트 폰)과 PlayerController(플레이어 컨트롤러)를 설정할 수 있다.
 *  설정된 Controller가 Pawn을 빙의,소유(Possess)하고 컨트롤러의 입력(Input)에 따라 폰이 영향 받는다.
 */
UCLASS()
class PROJECT_ANIMAGUS_API ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    virtual void StartPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
	ABattleGameMode();

    void SpawnPlayers();
    void ActivateInput();
	
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Settings")
    float start_time; // ex) 5초 후에 입력 활성화

    // 플레이어 ID를 (Key)로 스폰위치(Location, Rotation)를 (Value)로 갖음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Settings")
    TMap<int32, FTransform> spawn_transform;

private:
    // BP_AI 애셋
    TSubclassOf<class AAIController> AIControllerClass;
    TSubclassOf<class APawn> AIPlayerClass;

    // 라운드 경과 시간 출력
    FTimerHandle battle_timer_handle;
    float elasped_time;

    void InitBattleMode();
    void PrintElapsedtime();
};
