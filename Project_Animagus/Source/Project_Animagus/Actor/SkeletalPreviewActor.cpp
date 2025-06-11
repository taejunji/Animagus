// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletalPreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
// Sets default values
ASkeletalPreviewActor::ASkeletalPreviewActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 루트 설정
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // SkeletalMeshComponent 생성
    PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
    PreviewMesh->SetupAttachment(Root);
    // 애니메이션 모드 설정
    PreviewMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

}

void ASkeletalPreviewActor::SetMeshAndPlay(USkeletalMesh* Mesh, UAnimSequence* IdleAnimation)
{
    if (!PreviewMesh) return;
    PreviewMesh->SetSkeletalMesh(Mesh);
    if (IdleAnimation)
    {
        PreviewMesh->PlayAnimation(IdleAnimation, true);
    }

}

// Called when the game starts or when spawned
void ASkeletalPreviewActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkeletalPreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

