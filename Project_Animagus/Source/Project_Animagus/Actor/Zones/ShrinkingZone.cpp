#include "ShrinkingZone.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "../../Character/BaseCharacter.h"

AShrinkingZone::AShrinkingZone()
{
	PrimaryActorTick.bCanEverTick = true;

	// 설정: 축소 속도, 초기 반지름 등
	ShrinkSpeed = 100.0f;
	InitialRadius = 18000.0f;
	CurrentRadius = InitialRadius;
    Is_in = false;
    
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

    // SetActorScale3D(FVector(NewScale, NewScale, 1.0f));

	// Niagara 시스템에 LargeRadius 값 업데이트
	if (NiagaraComp)
	{
	    if (CurrentRadius > 5000.f)
	    {
	        NiagaraComp->SetVariableFloat(FName("LargeRa"), CurrentRadius + 5000.f);
	    }
	    else
        {
          NiagaraComp->SetVariableFloat(FName("HandleRa"), 10000.f - CurrentRadius);   
        }
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


    APawn* MyPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    ABaseCharacter* BCharacter = Cast<ABaseCharacter>(MyPawn);

    if (BCharacter->GetIsDead())
    {
        SetFogPostProcess(0);
        return;
    }
		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			if (!IsActorInsideZone(MyPawn))
			{
			    if (!Is_in)
			    {
			        if (EnterZoneSound)
			        {
			            UGameplayStatics::PlaySound2D(this, EnterZoneSound);
			            UE_LOG(LogTemp, Log, TEXT("SHRINKSOUND ON") );
			        }
			        
			        Is_in = true;
			        SetFogPostProcess(1);

			    }
                else
                {
                    UE_LOG(LogTemp, Log, TEXT("SHRINKIn and Is_in true") ); 
                } 
				ApplyGasDamage(MyPawn, DeltaTime);
			   
			}
		    else
		    {
		        if (Is_in)
		        {
		            UE_LOG(LogTemp, Log, TEXT("SHRINKout and Is_in true") ); 
		            Is_in = false;
		            SetFogPostProcess(0);
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

void AShrinkingZone::SetFogPostProcess(float NewWeight)
{
    TArray<AActor*> FoundVolumes;

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetFogPostProcess: World is null."));
        return;
    }
    
    UGameplayStatics::GetAllActorsOfClass(World, APostProcessVolume::StaticClass(), FoundVolumes);
    
    for (AActor* Actor : FoundVolumes)
    {
        APostProcessVolume* PPVolume = Cast<APostProcessVolume>(Actor);
        if (PPVolume)
        {
            {
                // WeightedBlendables 배열의 크기가 3 이상이어야 합니다.
                if (PPVolume->Settings.WeightedBlendables.Array.Num() >= 3)
                {
                    PPVolume->Settings.WeightedBlendables.Array[2].Weight = NewWeight;
                    UE_LOG(LogTemp, Log, TEXT("SetFogPostProcess: Updated material at index 2 to weight: %f"), NewWeight);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("SetFogPostProcess: WeightedBlendables array has less than 3 elements."));
                }
            }
        }
    }
    
}