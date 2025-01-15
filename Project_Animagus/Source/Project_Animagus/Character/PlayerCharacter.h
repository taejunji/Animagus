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

    void InitializeTPSSettings(); 

protected:
    virtual void BeginPlay() override;
        
private:
    void InitPlayerMesh();
};
