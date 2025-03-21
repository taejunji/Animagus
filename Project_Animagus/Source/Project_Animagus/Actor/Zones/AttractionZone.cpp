// Fill out your copyright notice in the Description page of Project Settings.


#include "AttractionZone.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../../GameMode/BattleGameMode.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Project_Animagus/Character/BaseCharacter.h"

AAttractionZone::AAttractionZone()
{
	PrimaryActorTick.bCanEverTick = true;

	// 박스 형태의 충돌 영역 생성 및 초기화
	ZoneCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneCollision"));
	ZoneCollision->InitBoxExtent(FVector(1000.0f, 1000.0f, 500.0f));
	ZoneCollision->SetCollisionProfileName(TEXT("Trigger"));
	RootComponent = ZoneCollision;

	// Niagara 컴포넌트 생성 및 부착
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComponent);

	// 기본적으로 담당할 플레이어 인덱스는 0
	ControlledPlayerIndex = 0;

	// DamagePerSecond 설정 (예: 초당 10)
	DamagePerSecond = 10.0f;
}

void AAttractionZone::BeginPlay()
{
	Super::BeginPlay();
}

void AAttractionZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAttractionParameters(DeltaTime);
}

bool AAttractionZone::IsActorInsideZone(AActor* Actor) const
{
	if (!Actor)
		return false;
	return ZoneCollision->IsOverlappingActor(Actor);
}

void AAttractionZone::ApplyGasDamage(AActor* AffectedActor, float DeltaTime)
{
	if (AffectedActor)
	{
		UGameplayStatics::ApplyDamage(AffectedActor, DamagePerSecond * DeltaTime, nullptr, this, nullptr);
	}
}

void AAttractionZone::UpdateAttractionParameters(float DeltaTime)
{
	// BattleGameMode에서 SpawnedPlayers 배열을 얻어옴
	ABattleGameMode* BM = Cast<ABattleGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!BM)
	{
		 UE_LOG(LogTemp, Warning, TEXT("AttractionZone: GameMode is not ABattleGameMode."));
		return;
	}

	if (!BM->SpawnedPlayers.IsValidIndex(ControlledPlayerIndex))
	{
		// UE_LOG(LogTemp, Warning, TEXT("AttractionZone: Invalid ControlledPlayerIndex %d"), ControlledPlayerIndex);
		return;
	}

	AActor* PlayerActor = BM->SpawnedPlayers[ControlledPlayerIndex];
	if (!PlayerActor)
		return;

	// 플레이어가 영역 내에 있으면 Niagara 파라미터 업데이트 및 데미지 적용
	if (IsActorInsideZone(PlayerActor))
	{
		if (NiagaraComp)
		{
			NiagaraComp->SetVariableFloat(FName("Attack"), 1.0f);
			NiagaraComp->SetVariableVec3(FName("Player"), PlayerActor->GetActorLocation());
			UE_LOG(LogTemp, Log, TEXT("AttractionZone: Player %d inside zone. Attack=1, Player position updated."), ControlledPlayerIndex);
		}
		// 로컬 컨트롤된 플레이어일 경우에만 데미지 적용
		APawn* Pawn = Cast<APawn>(PlayerActor);
		if (Pawn && Pawn->IsLocallyControlled())
		{
			ApplyGasDamage(PlayerActor, DeltaTime);
		}
	}
	else
	{
		if (NiagaraComp)
		{
			NiagaraComp->SetVariableFloat(FName("Attack"), 0.0f);
		}
	}
}

