// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ConnectGameMode.generated.h"

class ASkeletalPreviewActor;
class AConnectPlayerController;

/**
 * 캐릭터 선택 단계용 GameMode
 */
UCLASS()
class PROJECT_ANIMAGUS_API AConnectGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    
    ASkeletalPreviewActor* GetPreviewActor() const { return PreviewActor; }

    // PostLogin을 잡아서, 로그인(스폰) 직후 시작 위치를 고정
    virtual void PostLogin(APlayerController* NewPlayer) override;
protected:
    /** PreviewActor용 클래스 지정 (BP에서 할당) */
    UPROPERTY(EditAnywhere, Category="Preview")
    TSubclassOf<ASkeletalPreviewActor> PreviewActorClass;

    UPROPERTY()
    ASkeletalPreviewActor* PreviewActor;
    
    /** PreviewActor 스폰 위치 */
    UPROPERTY(EditAnywhere, Category="Preview")
    FVector PreviewSpawnLocation = FVector::ZeroVector;

    /** PreviewActor 스폰 회전 */
    UPROPERTY(EditAnywhere, Category="Preview")
    FRotator PreviewSpawnRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, Category="Spawn")
    FVector SpawnLocation = FVector(0.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, Category="Spawn")
    FRotator SpawnRotation = FRotator(0.f, 90.f, 0.f);
};