// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkeletalPreviewActor.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;

UCLASS()
class PROJECT_ANIMAGUS_API ASkeletalPreviewActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkeletalPreviewActor();

    UFUNCTION()
    void SetMesh(USkeletalMesh* Mesh);
    
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnSelectMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    USkeletalMeshComponent* GetPreviewMesh() { return PreviewMesh;  }
    
protected:
    
    UPROPERTY(VisibleAnywhere)
    USceneComponent* Root;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
    USkeletalMeshComponent* PreviewMesh;

    UPROPERTY(EditAnywhere, Category="Animation")
    UAnimSequence* IdleAnimation;

    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<class UAnimMontage> select_montage;

    
    // UPROPERTY(EditAnywhere, Category="Animation")
    // UAnimMontage* SelectMontage;
    //
    // UPROPERTY(EditAnywhere, Category="Animation")
    // UAnimMontage* IdleMontage;

};
