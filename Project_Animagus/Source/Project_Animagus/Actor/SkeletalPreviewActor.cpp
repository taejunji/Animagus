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
    // PreviewMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

}

void ASkeletalPreviewActor::SetMesh(USkeletalMesh* Mesh)
{
    if (!PreviewMesh || !Mesh) return;

    PreviewMesh->SetSkeletalMesh(Mesh);

    UAnimInstance* AnimInst = PreviewMesh->GetAnimInstance();
    if (AnimInst && select_montage)
    {
        // Duration은 **선택한 몽타주 전체의 총 재생 시간(초)**
        // 만약 재생에 실패하면 0.0f를 반환
        float Duration = AnimInst->Montage_Play(select_montage);
        if (Duration > 0.f)
        {
            AnimInst->Montage_JumpToSection(FName("SelectSection"), select_montage);
        }
    }

    //if (IdleAnimation)
    //{
    //    PreviewMesh->PlayAnimation(IdleAnimation, true);
    //}
    
    // UAnimInstance* AnimInst = PreviewMesh->GetAnimInstance();
    // if (AnimInst && SelectMontage)
    // {
    //     // 1) 선택 몽타주 재생 (Blend In/Out 설정은 몽타주 에디터에서)
    //     AnimInst->Montage_Play(SelectMontage);
    //
    //     // 2) 끝나면 IdleMontage로 전환
    //     FOnMontageEnded EndDelegate;
    //     EndDelegate.BindUFunction(this, FName("OnSelectMontageEnded"));
    //     AnimInst->Montage_SetEndDelegate(EndDelegate, SelectMontage);
    // }
    // else if (AnimInst && IdleMontage)
    // {
    //     // Select 없으면 바로 Idle 루프 재생
    //     AnimInst->Montage_Play(IdleMontage);
    // }
}


void ASkeletalPreviewActor::OnSelectMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!PreviewMesh) return;

    UAnimInstance* AnimInst = PreviewMesh->GetAnimInstance();
    // if (AnimInst && IdleMontage)
    // {
    //     // Idle 몽타주 재생 (loop)
    //     AnimInst->Montage_Play(IdleMontage);
    // }
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

