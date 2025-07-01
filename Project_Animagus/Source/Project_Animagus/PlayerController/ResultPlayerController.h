// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ResultPlayerController.generated.h"

class UResultWidget;
class AResultCharacterActor;
class UAnimSequence;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class PROJECT_ANIMAGUS_API AResultPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<UResultWidget> ResultWidgetClass;

    UPROPERTY()
    UResultWidget* ResultWidget;

    UPROPERTY(EditAnywhere, Category="Result")
    TSubclassOf<AResultCharacterActor> ResultActorClass;

    UPROPERTY(EditAnywhere, Category="Result")
    UAnimSequence* WinnerAnimation;

    UPROPERTY(EditAnywhere, Category="Result")
    UAnimSequence* LoserAnimation;

    UPROPERTY(EditAnywhere, Category="Result")
    FTransform ResultActorSpawnTransform = FTransform(FRotator::ZeroRotator, FVector::ZeroVector);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Result")
    bool bIsWin;

    UPROPERTY(EditAnywhere, Category="Audio")
    USoundBase* WinnerBGM;

    UPROPERTY(EditAnywhere, Category="Audio")
    USoundBase* LoserBGM;
    
    // 꽃가루
    UPROPERTY(EditAnywhere, Category="VFX")
    UNiagaraSystem* ConfettiFX;

    // 꽃가루 위치
    UPROPERTY(EditAnywhere, Category="VFX")
    TArray<FTransform> ConfettiSpawnTransforms;

    // 비이펙트
    UPROPERTY(EditAnywhere, Category="VFX")
    UNiagaraSystem* LostFX;

    //비 
    UPROPERTY(EditAnywhere, Category="VFX")
    TArray<FTransform> LostFXSpawnTransforms;

    // 졌을때 라이트 강도
    UPROPERTY(EditAnywhere, Category="Lighting")
    float LostLightIntensity = 1.0f;

    // 졌을때 라이트 색깔
    UPROPERTY(EditAnywhere, Category="Lighting")
    FLinearColor LostLightColor = FLinearColor(0.5f, 0.5f, 0.5f);
};
