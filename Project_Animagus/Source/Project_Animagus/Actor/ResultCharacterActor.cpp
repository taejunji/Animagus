// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultCharacterActor.h"

// Sets default values
AResultCharacterActor::AResultCharacterActor()
{
    PrimaryActorTick.bCanEverTick = false;
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComp");
    RootComponent = MeshComp;
    MeshComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);

}

// Called when the game starts or when spawned
void AResultCharacterActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AResultCharacterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AResultCharacterActor::Initialize(USkeletalMesh* Mesh, UAnimSequence* OutcomeAnim)
{
    if (!MeshComp || !Mesh) return;

    MeshComp->SetSkeletalMesh(Mesh);

    if (OutcomeAnim)
    {
        // 논루프 재생
        MeshComp->PlayAnimation(OutcomeAnim, true);
    }
}
