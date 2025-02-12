// Fill out your copyright notice in the Description page of Project Settings.


#include "ShrinkingZone.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AShrinkingZone::AShrinkingZone()
{
    PrimaryActorTick.bCanEverTick = true;

    ShrinkSpeed = 100.0f;
    InitialRadius = 15000.0f;
    CurrentRadius = InitialRadius;

    // Collision Component 생성 및 설정
    CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
    CollisionComp->InitCapsuleSize(InitialRadius, 5000.0f);  // 높이는 넉넉하게
    RootComponent = CollisionComp;

    // Niagara Component 생성 및 연결 (에디터에서 Niagara System 할당)
    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
    NiagaraComp->SetupAttachment(RootComponent);
}

void AShrinkingZone::BeginPlay()
{
    Super::BeginPlay();
}

void AShrinkingZone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 맵 축소: 시간에 따라 CurrentRadius를 감소
    CurrentRadius -= ShrinkSpeed * DeltaTime;
    if (CurrentRadius < 0.0f)
    {
        CurrentRadius = 0.0f;
    }
    UE_LOG(LogTemp, Warning, TEXT("CurrentRadius: %f"), CurrentRadius);
    
    // Cylinder Collision 및 Niagara Component의 스케일 업데이트
    float NewScale = CurrentRadius / InitialRadius;
    SetActorScale3D(FVector(NewScale, NewScale, 1.0f));
    
    // Niagara 시스템의 User Parameter("LargeRadius") 업데이트
    if (NiagaraComp)
    {
        NiagaraComp->SetVariableFloat(FName("LargeRadius"), CurrentRadius);
    }

    // 모든 플레이어에 대해 안전 영역 판별 및 데미지 처리
    TArray<AActor*> PlayerActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), PlayerActors);
    for (AActor* Actor : PlayerActors)
    {
        if (!IsActorInsideZone(Actor))
        {
            ApplyGasDamage(Actor, DeltaTime);
        }
    }

    // 디버그 모드가 활성화 되어 있다면 Cylinder 충돌체를 눈으로 확인할 수 있도록 DrawDebugCylinder 실행
    if (bShowDebug)
    {
        // Cylinder의 중심(Zone 중심)
        FVector ZoneCenter = GetActorLocation();
        // Cylinder의 높이 (예: 충분히 큰 값)
        float Height = 10000.0f;
        // Cylinder의 위, 아래 끝 위치 계산 (Z축 방향)
        FVector Start = ZoneCenter - FVector(0, 0, Height * 0.5f);
        FVector End = ZoneCenter + FVector(0, 0, Height * 0.5f);
        
        // DrawDebugCylinder(World, Start, End, Radius, Segments, Color, bPersistentLines, LifeTime, DepthPriority)
        UE_LOG(LogTemp, Warning, TEXT("CurrentRadius_2: %f"), CurrentRadius);
        DrawDebugCylinder(GetWorld(), Start, End, CurrentRadius, 32, FColor::Green, false, 2.0f, 0, 5.0f);
        DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0f, 12, FColor::Red, false, 2.0f);
    }
}

bool AShrinkingZone::IsActorInsideZone(AActor* OtherActor) const
{
    if (!OtherActor)
        return false;

    FVector ActorLocation = OtherActor->GetActorLocation();
    FVector ZoneCenter = GetActorLocation();
    float Distance = FVector::Dist2D(ActorLocation, ZoneCenter);
    return (Distance <= CurrentRadius);
}

void AShrinkingZone::ApplyGasDamage(AActor* AffectedActor, float DeltaTime)
{
    // 예시 데미지: 5.0f * DeltaTime (필요에 따라 조절)
    UGameplayStatics::ApplyDamage(AffectedActor, 5.0f * DeltaTime, nullptr, this, nullptr);
}

