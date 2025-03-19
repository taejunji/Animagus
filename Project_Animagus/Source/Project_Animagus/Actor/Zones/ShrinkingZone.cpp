#include "ShrinkingZone.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"

AShrinkingZone::AShrinkingZone()
{
	PrimaryActorTick.bCanEverTick = true;

	// 설정: 축소 속도, 초기 반지름 등
	ShrinkSpeed = 100.0f;
	InitialRadius = 18000.0f;
	CurrentRadius = InitialRadius;

	// Collision Component 생성 및 설정
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
	CollisionComp->InitCapsuleSize(InitialRadius, 5000.0f);
	RootComponent = CollisionComp;

	// Niagara Component 생성 및 연결
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

	// LargeRadius(현재 CurrentRadius)를 ShrinkSpeed에 따라 줄임
	CurrentRadius -= ShrinkSpeed * DeltaTime;
	if (CurrentRadius < 1500.0f)
	{
		CurrentRadius = 1500.0f;
	}
	// UE_LOG(LogTemp, Log, TEXT("Tick: CurrentRadius = %f"), CurrentRadius);

	// 액터의 스케일을 업데이트 (LargeRadius 기준, Z는 1)
	float NewScale = CurrentRadius / InitialRadius;
	SetActorScale3D(FVector(NewScale, NewScale, 1.0f));

	// Niagara 시스템에 LargeRadius 값 업데이트
	if (NiagaraComp)
	{
		NiagaraComp->SetVariableFloat(FName("LargeRadius"), CurrentRadius);
	}

	// 디버그: 현재 안전구역(현재 LargeRadius) 시각화
	if (bShowDebug)
	{
		FVector ZoneCenter = GetActorLocation();
		float Height = 10000.0f;
		FVector Start = ZoneCenter - FVector(0, 0, Height * 0.5f);
		FVector End = ZoneCenter + FVector(0, 0, Height * 0.5f);
		DrawDebugCylinder(GetWorld(), Start, End, CurrentRadius, 32, FColor::Green, false, 0.2f, 0, 5.0f);
		DrawDebugSphere(GetWorld(), ZoneCenter, 50.0f, 12, FColor::Red, false, 0.2f);
	}

	// 모든 플레이어에 대해 안전 영역 판별 및 데미지 적용
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), PlayerActors);
	for (AActor* Actor : PlayerActors)
	{
		APawn* Pawn = Cast<APawn>(Actor);
		if (Pawn && Pawn->IsLocallyControlled())
		{
			if (!IsActorInsideZone(Pawn))
			{
				ApplyGasDamage(Pawn, DeltaTime);
			}
		}
	}
}

bool AShrinkingZone::IsActorInsideZone(AActor* OtherActor) const
{
	if (!OtherActor)
		return false;

	FVector ActorLocation = OtherActor->GetActorLocation();
	FVector ZoneCenter = GetActorLocation();
	float Distance = FVector::Dist2D(ActorLocation, ZoneCenter);
	// 안전구역 판단은 단순히 CurrentRadius를 사용함
	return (Distance <= CurrentRadius);
}

void AShrinkingZone::ApplyGasDamage(AActor* AffectedActor, float DeltaTime)
{
	UGameplayStatics::ApplyDamage(AffectedActor, 5.0f * DeltaTime, nullptr, this, nullptr);
}
