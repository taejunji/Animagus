// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"


enum class LevelType { Login, Lobby, Battle };
enum class CharacterMesh { Monkey, Tiger, Koala, Sheep };

struct StoredPlayerData
{
    CharacterMesh stored_mesh;
    // 사용 메쉬
    // 악세서리 메쉬 
    // 승패 전적 등등 
};

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
    int32 round_count;

    // 저장된 플레이어 데이터
    StoredPlayerData player_data;

    UPROPERTY(BlueprintReadOnly, Category = "Character Mesh") 
    TMap<FString, USkeletalMesh*> CharacterMeshes; 
    // 캐릭터 메쉬 에셋 데이터를 Map으로 저장 
    // -> 로비, 배틀 레벨에서 공통된 메쉬 데이터 사용하기 위해서 인스턴스에서 메쉬 정보를 기억하도록 한다
    // -> 애셋 데이터를 인스턴스 생성시 한번만 로드해서 데이터를 갖도록 한다

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
    int32 GetRoundCount() const { return round_count; }

public:
    // AI 캐릭터
    // TArray<class AAICharacter*> AIPlayers;

    // AI 추가 함수
    // void AddAICharacter(AAICharacter* AICharacter);
};
