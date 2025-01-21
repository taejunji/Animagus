// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class PROJECT_ANIMAGUS_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float default_walk_speed; // 기본 걷기 속도
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float default_run_speed; // 최대 달리기 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float current_speed; // 현재 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float speed_change_rete; // 속도 변화 비율 ( 30.f면 1초에 30씩 변화 )


public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

    void SetWalkSpeed(float fValue);
};

// Called to bind functionality to input
// virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
// 
//void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}