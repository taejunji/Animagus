// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"
/**
 * 
 */

UCLASS()
class PROJECT_ANIMAGUS_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<class USpringArmComponent> spring_arm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<class UCameraComponent> camera;

public:
    APlayerCharacter();
    virtual void Tick(float DeltaTime) override;

    void Initialize_TPS_Settings(); // TPS 타입 컨트롤 변수 설정
    void Initialize_RPG_Settings(); // RPG 타입 컨트롤 변수 설정

protected:
    virtual void BeginPlay() override;
        
private:
    void InitPlayerMesh();
};
