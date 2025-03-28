// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

/**
 *  한 레벨 당 하나의 게임모드를 설정할 수 있으며 게임 모드는 DefaultPawn(디폴트 폰)과 PlayerController(플레이어 컨트롤러)를 설정할 수 있다.
 *  설정된 Controller가 Pawn을 빙의,소유(Possess)하고 컨트롤러의 입력(Input)에 따라 폰이 영향 받는다.
 */

class ABaseCharacter;

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

    // 현재 카운트다운 시간
    float CurrentCountdownTime;

    // 현재 라운드 진행 시간 (카운트다운 종료 후)
    float CurrentRoundTime;

    // 타이머 핸들들
    FTimerHandle CountdownTimerHandle;
    FTimerHandle RoundTimerHandle;

    // 타이머 업데이트 함수
    void CountdownTimerUpdate();
    void RoundTimerUpdate();
    
private:
    // BP_AI 애셋
    TSubclassOf<class AAIController> AIControllerClass;
    TSubclassOf<class APawn> AIPlayerClass;

    // 라운드 경과 시간 출력
    FTimerHandle battle_timer_handle;
    float elasped_time;

    void InitBattleMode();
    void PrintElapsedtime();
 

public:   
    // 스폰된 플레이어 캐릭터들을 저장할 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning", meta = (AllowPrivateAccess = "true"))
    TArray<ABaseCharacter*> SpawnedPlayers;

    // 플레이어 스폰 위치 배열 (에디터에서 조절 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Spawning", meta = (AllowPrivateAccess = "true"))
    TArray<FVector> SpawnLocations;

    // 플레이어 스폰 회전 배열 (에디터에서 조절 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning", meta = (AllowPrivateAccess = "true"))
    TArray<FRotator> SpawnRotations;
    
    // 내가 소유할 플레이어 인덱스 (예: GameInstance에서 가져올 수 있음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning", meta = (AllowPrivateAccess = "true"))
    int32 PossessIndex;
};
