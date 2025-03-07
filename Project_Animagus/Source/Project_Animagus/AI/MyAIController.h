// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API AMyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AMyAIController(const FObjectInitializer& ObjectInitializer);

    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere)
    TObjectPtr<class UBehaviorTree> AIBehavior;


};