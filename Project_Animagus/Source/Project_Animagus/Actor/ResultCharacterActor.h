// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResultCharacterActor.generated.h"

class USkeletalMeshComponent;
class UAnimSequence;

UCLASS()
class PROJECT_ANIMAGUS_API AResultCharacterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResultCharacterActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void Initialize(USkeletalMesh* Mesh, UAnimSequence* OutcomeAnim);

protected:
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, Category="Animation")
    UAnimSequence* WinnerAnimation;

    UPROPERTY(EditAnywhere, Category="Animation")
    UAnimSequence* LoserAnimation;

};
