// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldActor.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Projectile/ProjectileBase.h"

AShieldActor::AShieldActor()
{
	PrimaryActorTick.bCanEverTick = true;

	// 보호막 충돌 영역: SphereComponent 생성 및 초기화
	ShieldCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ShieldCollision"));
	ShieldCollision->InitSphereRadius(150.f);
    
	ShieldCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // 보호막 액터는 기본적으로 WorldDynamic으로 설정
	ShieldCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    ShieldCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

    RootComponent = ShieldCollision;

	// 보호막 시각 효과용 Niagara 컴포넌트 생성 및 부착
	ShieldEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShieldEffect"));
	ShieldEffect->SetupAttachment(RootComponent);
	// 보호막 Niagara 효과 에셋은 BP에서 할당

	// 기본 지속 시간
	ShieldDuration = 5.0f;

	// 초기 ShieldOwner는 nullptr
	ShieldOwner = nullptr;

	// 보호막 충돌 이벤트 바인딩
    ShieldCollision->OnComponentBeginOverlap.AddDynamic(this, &AShieldActor::OnShieldOverlapBegin);
}

void AShieldActor::BeginPlay()
{
	Super::BeginPlay();

	// 보호막 지속 시간 후에 자동으로 파괴되도록 타이머 설정
	FTimerHandle TempHandle;
    GetWorld()->GetTimerManager().SetTimer(TempHandle, FTimerDelegate::CreateLambda([this]()
    {
        this->Destroy();
    }), ShieldDuration, false);
    
}

void AShieldActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 만약 ShieldOwner가 유효하면, 보호막 액터를 ShieldOwner의 위치에 따라 업데이트 (따라다님)
	if (ShieldOwner)
	{
		// 약간의 오프셋
		FVector NewLocation = ShieldOwner->GetActorLocation();
		SetActorLocation(NewLocation);
	}
}

void AShieldActor::OnShieldOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                        bool bFromSweep, const FHitResult & SweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("ShieldActor: OnShieldOverlapBegin triggered for Actor: %s"),
           OtherActor ? *OtherActor->GetName() : TEXT("null"));

    if (!OtherActor || OtherActor == this)
        return;

    // 투사체와의 충돌 처리
    AProjectileBase* Projectile = Cast<AProjectileBase>(OtherActor);
    if (Projectile)
    {
        FString ShooterName = Projectile->Shooter ? Projectile->Shooter->GetName() : TEXT("NULL");
        FString ShieldOwnerName = ShieldOwner ? ShieldOwner->GetName() : TEXT("NULL");
        UE_LOG(LogTemp, Warning, TEXT("ShieldActor Overlap: Projectile=%s | Shooter=%s | ShieldOwner=%s"), 
            *OtherActor->GetName(), *ShooterName, *ShieldOwnerName);
        
        // 내 보호막(ShieldOwner와 동일)과 충돌 시 무시
        if (Projectile->Shooter == ShieldOwner || Projectile->Shooter == NULL)
        {
            UE_LOG(LogTemp, Warning, TEXT("ShieldActor: Ignoring overlap with owner's projectile."));
            ShieldCollision->IgnoreActorWhenMoving(OtherActor, true);
            return;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ShieldActor: Overlap with projectile from other player: %s"), *OtherActor->GetName());
            // 다른 플레이어의 투사체와 충돌하면 투사체를 Destroy() 시킵니다.
            // Projectile->DestroySkill();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldActor: Overlap with non-projectile actor: %s"), *OtherActor->GetName());
        // 필요에 따라 추가 처리 (예: 일반 액터 제거 등)
    }
}


void AShieldActor::SetShieldOwner(AActor* NewOwner)
{
	ShieldOwner = NewOwner;
}

