// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Character/PlayerCharacter.h"

#include "../Server/Server/protocol.h"

#include "BattleGameMode.generated.h"

/**
 *  한 레벨 당 하나의 게임모드를 설정할 수 있으며 게임 모드는 DefaultPawn(디폴트 폰)과 PlayerController(플레이어 컨트롤러)를 설정할 수 있다.
 *  설정된 Controller가 Pawn을 빙의,소유(Possess)하고 컨트롤러의 입력(Input)에 따라 폰이 영향 받는다.
 */

class ABaseCharacter;
class AItem_Box_Base;
class USoundBase;
class AShrinkingZone;
class AAttractionZone;
class UAudioComponent;
class USkillSelectionWidget;

UCLASS()
class PROJECT_ANIMAGUS_API ABattleGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void StartPlay() override;
    virtual void Tick(float DeltaTime) override;

public:    // 게임 시스템 관련
    ABattleGameMode();
    void InitBattleMode();
    void ActivateInput();

    // 타이머 업데이트 함수
    void PrintElapsedtime();
    void CountdownTimerUpdate();
    void RoundTimerUpdate();
    void HandleServerTime(uint64 server_time);
    float GetCurrentRoundTime() const { return CurrentRoundTime; }

    float TIME_OVER = 60.0f * 0.5f;    // 라운드 종료 시간

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Settings")
    float start_time;               // 라운드 시작 시간 (안씀)
    uint64 StartTime2Server = 0;    // 라운드 시작 시간
    float CurrentCountdownTime;     // 현재 카운트다운 시간
    float CountdownTime = 5.0f;
    uint64 CurrentRoundTime;        // 현재 라운드 진행시간
    float elasped_time;             // 현재 라운드 진행시간 (안씀)
    int32 SelectionTimeRemaining;   // 현재 스킬 선택 남은시간
    int32 SelectionTime = 10;

    // 시간별 이벤트 토글
    bool CalledActiveInput = false;
    bool CalledConfirmInput = false;

    // 타이머 핸들들
    FTimerHandle SkillSelectionTimerHandle; // 스킬선택 남은시간 갱신 (안씀)
    FTimerHandle SkillSelectionTickHandle;  // 스킬선택 남은시간 갱신
    FTimerHandle CountdownTimerHandle;      // 카운트다운 갱신
    FTimerHandle GameStartTimerSoundHandle; // 카운트다운 사운드 재생
    FTimerHandle RoundTimerHandle;          // 라운드 진행시간 갱신
    FTimerHandle battle_timer_handle;       // 라운드 진행시간 갱신 (안씀)


public:    // 게임 오브젝트 관련
    void SpawnPlayers();
    void SpawnAIPlayers(Protocol::SC_AI_SPAWN_PKT& pkt);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes)
    TSubclassOf<APawn> NetPawnClass;

    const uint16 MAX_PLAYER = 8;
    uint16 CurrentPlayerCount = 1;

    // 플레이어 ID를 (Key)로 스폰위치(Location, Rotation)를 (Value)로 갖음
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle Settings")
    TMap<int32, FTransform> spawn_transform;
    // 스폰된 플레이어 캐릭터들을 저장할 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
    TMap<int32, ABaseCharacter*> SpawnedPlayers;
    //TArray<ABaseCharacter*> SpawnedPlayers;
    // 플레이어 스폰 위치 배열 (에디터에서 조절 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
    TArray<FVector> SpawnLocations;
    // 플레이어 스폰 회전 배열 (에디터에서 조절 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
    TArray<FRotator> SpawnRotations;
    // 내가 소유할 플레이어 인덱스 (예: GameInstance에서 가져올 수 있음)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
    int32 PossessIndex;

    APlayerCharacter* PlayerCharacter; // 플레이어 캐릭터 포인터
    // 관전 시 순회를 위한 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
    TArray<ABaseCharacter*> IndexingSpawnedPlayers;



    UFUNCTION(BlueprintCallable, Category = "PowerUp")
    void InitializeArea1SpawnPoints();

    UFUNCTION(BlueprintCallable, Category = "PowerUp")
    void SpawnItemsInArea1();

    UFUNCTION(BlueprintCallable, Category = "PowerUp")
    void InitializeArea2SpawnPoints();

    UFUNCTION(BlueprintCallable, Category = "PowerUp")
    void InitializeArea3SpawnPoints();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<FVector> Area1SpawnPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<FVector> Area2SpawnPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    TArray<FVector> Area3SpawnPoints;

    TArray<TArray<FVector>> AreaSpawnPoints;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerUp")
    TArray<class AItem_Box_Base*> SpawnedItems;


public:    // 네트워크 서비스 관련
    void SetPlayerIndex(uint16 playerIndex);
    void SpawnPlayer(Protocol::SC_SPAWN_PKT& pkt);
    void MoveOtherPlayer(Protocol::CS_MOVE_PKT& pkt);
    void SpawnSkill(Protocol::CS_USING_SKILL_PKT& pkt);
    void SpawnItem(Protocol::SC_SPAWN_ITEM_PKT& pkt);
    void SpawnItemsInArea3(Protocol::SC_SPAWN_ITEM_PKT& pkt);
    void UpdateHp(Protocol::SC_UPDATE_HP_PKT& pkt);


    bool AmIHost = false;
    int16 RoomId = 0;
    int16 PlayerIndex = 0;

protected:



    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* BackgroundMusic;

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* CountSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* StartSound;

public:
    
    /** 로딩 UI 위젯 클래스 (에디터에서 할당) */
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> LoadingWidgetClass;

    /** 인스턴스화된 로딩 UI */
    UPROPERTY()
    UUserWidget* LoadingWidget;

    /** 5초 후 호출될 초기화 콜백 */
    UFUNCTION()
    void OnPostLoadInitialize();

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* AttractSoundWave;
    
    // BP_AI 애셋
    UPROPERTY(EditAnywhere, Category = "AI")
    TSoftClassPtr<class AAIController> AIControllerClass;

    UPROPERTY(EditAnywhere, Category = "AI")
    TSubclassOf<class APawn> AIPlayerClass;

    UPROPERTY(EditAnywhere, Category = "Item")
    TSubclassOf<class APowerUpItem> PowerUpBpclass;

    UPROPERTY(EditAnywhere, Category = "Item")
    TSubclassOf<class AItem_Box_Base> ItemBoxBpclass;

    TSubclassOf<class AAttractionZone> AttractionBpclass;

    TSubclassOf<class AItem_Box_High> ItemBoxHighBpclass;

    UPROPERTY()
    UAudioComponent* BackgroundMusicComponent;
    void PlayBackgroundMusic();
    
    UPROPERTY(EditAnywhere, Category = "ShrinkZone")
    TSubclassOf<class AShrinkingZone> ShrinkzoneBpclass;

public:
    UPROPERTY()
    USkillSelectionWidget* ActiveSkillSelectionWidget;


    UFUNCTION()
    void OnSkillSelectionTimeout();

    /** 1초마다 호출, UI에 남은 시간 전파 */
    UFUNCTION()
    void OnSkillSelectionTick();
    
public:
    AShrinkingZone* ShrinkingZone;

    TArray<AAttractionZone*> AttractionZones;

};
