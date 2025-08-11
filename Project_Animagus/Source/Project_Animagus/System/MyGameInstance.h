// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../Project_Animagus.h"
#include "../Server/Server/protocol.h"
#include "MyGameInstance.generated.h"


enum class LevelType { Login, Lobby, Battle };


enum class CharacterMesh
{
    Monkey,
    Koala,
    Sheep,
    Fox,
    Sloth,
    Elephant,
    Raccoon,
    Deer,
    Cow,
    Unicorn,
    Zebra,
    Donkey,

    Count
};

struct StoredPlayerData
{
    CharacterMesh stored_mesh;
    // 사용 메쉬
    // 악세서리 메쉬 
    // 승패 전적 등등 
};
class UAudioComponent;
class USoundBase;
// ------------------------------------------------------------------------------
// 게임 인스턴스는 프로그램이 시작할 때 생기는 싱글 톤 객체
// 플레이어가 DB로 갖던 정보를 동기화하거나 레벨 간( 로그인, 로비, 배틀 ) 공유 데이터를 관리할 수 있다.
// 
// 아래는 언리얼의 계층? 구조로 상위 객체의 참조를 호출해서 상호작용 할 수 있다.
// 
// GameInstance                 <- 전역 싱글톤 
//      GameMode                <- 레벨에 속한 게임 모드 
//          PlayerController    <- 플레이어 컨트롤러 
//           DefaultPawn        <- 플레이어
// ------------------------------------------------------------------------------
//  로그인 후에 DB에 저장된 플레이어 데이터를 게임 인스턴스에서 저장하여 언제든 접근하도록 할 것
// -------------------------------------------------------------------------------
UCLASS()
class PROJECT_ANIMAGUS_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    // 현재 레벨 타입
    LevelType current_level;

    // 라운드 카운트
    UPROPERTY(BlueprintReadOnly, Category = "Round")
    int32 round_count;

    // 저장된 플레이어 데이터
    StoredPlayerData player_data;

   UPROPERTY(BlueprintReadOnly, Category = "Character Mesh") 
    TMap<FString, USkeletalMesh*> CharacterMeshes; 
    // 캐릭터 메쉬 에셋 데이터를 Map으로 저장 
    // -> 로비, 배틀 레벨에서 공통된 메쉬 데이터 사용하기 위해서 인스턴스에서 메쉬 정보를 기억하도록 한다
    // -> 애셋 데이터를 인스턴스 생성시 한번만 로드해서 데이터를 갖도록 한다
    
    UPROPERTY(BlueprintReadWrite, Category="Settings")
    float MouseSensitivity = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category="Settings")
    int32 AimImageIndex = 0;

    UPROPERTY(BlueprintReadWrite, Category="Settings")
    int32 myRank = 1;

public:
    UMyGameInstance(const FObjectInitializer& ObjectInitializer);

    virtual void Init() override;         // 프로그램 시작 시 한번 호출
    virtual void Shutdown() override;     // 프로그램 종료 시 한번 호출

public:
    void InitGameInstance();            // 게임 시작 시 라운드 초기화 
    void PrintGameInstanceData();

    void SwitchLevel(LevelType level);  // 레벨 전환 
    void SetLevelType(LevelType level) { current_level = level; }
    void SetRoundCount(int32 new_round_count) { round_count = new_round_count; }

    LevelType GetLevelType() const { return current_level; }

    UFUNCTION(BlueprintCallable, Category = "Round")
    int32 GetRoundCount() const { return round_count; }

    FString GetPlayerName() const { return MyPlayerName; }

public:
    UFUNCTION(BlueprintCallable)
    void ConnectToGameServer();

    UFUNCTION(BlueprintCallable)
    void DisconnectFromGameServer();

    UFUNCTION(BlueprintCallable)
    void HandleRecvPackets();

    void SendPacket(SendBufferRef SendBuffer);


public:
    void HandleLobbyHost(Protocol::SC_UR_HOST_PKT& pkt);
    void HandleStartGame(Protocol::SC_START_GAME_PKT& pkt);
    void HandleEnterGame(Protocol::SC_ENTER_GAME_PKT& pkt);
    void HandleSpawn(Protocol::SC_SPAWN_PKT& pkt);
    void HandleMove(Protocol::CS_MOVE_PKT& pkt);
    void HandleSkill(Protocol::CS_USING_SKILL_PKT& pkt);
    void HandleSpawnItem(Protocol::SC_SPAWN_ITEM_PKT& pkt);
    void HandleUpdateHp(Protocol::SC_UPDATE_HP_PKT& pkt);
    void HandleBattleRoundEnd(Protocol::SC_ROUND_END_PKT& pkt);
    void HandleBattleRoundInit(Protocol::SC_ROUND_INIT_PKT& pkt);
    void HandleAISpawn(Protocol::SC_AI_SPAWN_PKT& pkt);
    void HandleJumpEffect(Protocol::CS_JUMP_EFT_PKT& pkt);
    void HandleBattleModeEnd(Protocol::SC_GAME_END_PKT& pkt);
    void HandleLoginSuccess(Protocol::SC_LOGIN_SUCC_PKT& pkt);
    void HandleLoginFail(Protocol::SC_LOGIN_FAIL_PKT& pkt);
    void HandleSignUpSuccess();
    void HandleSignUpFail();
    void HandleSetPowerUp(Protocol::SC_SET_POWERUP_PKT& pkt);
    void HandleRoomEnter();
    void HandleRoomEnterFail();
    void HandleAlivePlayerCount(Protocol::SC_PLAYER_COUNT_PKT& pkt);

    // 로그인 메인 룸 BGM
    UPROPERTY(EditDefaultsOnly, Category="Audio")
    USoundBase* LoginBGM;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    USoundBase* MenuBGM;

    // 재생용 컴포넌트
    UPROPERTY(Transient)
    UAudioComponent* LoginBGMComponent;

    UPROPERTY(BlueprintReadWrite, Category = "Settings")
    float MasterVolume = 1.0f;

    UPROPERTY()
    float LoginBGMPlaybackTime = 0.f; // 저장된 재생 위치

    UFUNCTION()
    void SaveBGMPlaybackTime();

    // 메뉴 BGM 재생
    void PauseLoginBGM();         
    void ResetLoginBGM(); 

    virtual void OnStart() override;
public:
    // AI 캐릭터
    // TArray<class AAICharacter*> AIPlayers;

    // AI 추가 함수
    // void AddAICharacter(AAICharacter* AICharacter);

    class FSocket* Socket;
    //FString IpAddress = TEXT("182.230.58.32");
    int16 Port = 7777;
    TSharedPtr<class Session> ClientSession;

    uint16 MyPlayerId = 0;
    bool    AmIHost = false; // 내가 호스트인지 여부
    FString MyPlayerName; // 플레이어 이름
};
