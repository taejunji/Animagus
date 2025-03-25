#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Project_Animagus/Actor/Shield/ShieldActor.h"

AProjectileBase::AProjectileBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // ProjectileBase.cpp의 생성자 내에서
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->InitSphereRadius(10.0f);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // 투사체 전용 커스텀 채널 (예: ECC_GameTraceChannel1)을 사용하도록 설정
    CollisionSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);

    // 기본적으로 모든 채널 무시하고, 필요한 채널만 Block 처리
    CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

    // 일반적인 맵 데이터와 Pawn에 대해서는 Block 처리
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
    CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

    // 보호막과의 충돌은 Overlap 이벤트로 처리하도록 설정
    // ShieldActor는 별도의 충돌 채널이나, 동일 채널에서 Overlap 이벤트를 발생시키도록 설정할 수 있음.
    CollisionSphere->SetGenerateOverlapEvents(true);
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnOverlapBegin);
    
    RootComponent = CollisionSphere;
    
    // PointLight 생성
    ProjectileLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ProjectileLight"));
    ProjectileLight->SetupAttachment(RootComponent);
    ProjectileLight->SetIntensity(3000.f);
    ProjectileLight->SetLightColor(FLinearColor::Red);

    // Niagara 파티클 컴포넌트 생성 (이동 중 효과)
    ProjectileEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffect"));
    ProjectileEffectComponent->SetupAttachment(RootComponent);

    // ProjectileMovement 컴포넌트 생성 및 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1500.f;
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.f;

    // 기본 값 설정
    DamageValue = 50.0f;
    Lifetime = 10.0f;  // 예를 들어, 5초 후 자동 소멸
    Shooter = nullptr; // 스폰 시 발사자 정보를 외부에서 설정
    KnockbackForce = 1000;
    // 충돌 이벤트 바인딩
    EffectScale = 1.0f;
    
    CollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}


void AProjectileBase::BeginPlay()
{
    Super::BeginPlay();

    if (!Shooter)
    {
        Shooter = GetInstigator();
    }
    
    // 스폰 시 FlashEffect 이펙트 재생
    if (FlashEffect && ProjectileEffectComponent)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            FlashEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }

    if (ProjectileEffectComponent && ProjectileEffectComponent->GetAsset())
    {
        ProjectileEffectComponent->SetFloatParameter(FName("Scale"), EffectScale);
        ProjectileEffectComponent->Activate();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ProjectileEffectComponent has no asset assigned."));
    }

    
    // Lifetime 타이머 설정: Lifetime 후에 DestroySkill 호출
    //FTimerHandle LifetimeHandle;
    GetWorld()->GetTimerManager().SetTimer(
        LifetimeHandle,
        this,
        &AProjectileBase::DestroySkill,
        Lifetime,
        false
    );
}

void AProjectileBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AProjectileBase::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                              const FHitResult& Hit)
{
    // OtherActor가 유효하고, 자신(this) 및 Shooter와 다를 때만 처리
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {
        // 충돌 지점의 법선 벡터를 회전값으로 변환 (충돌 표면이 외부를 향하도록)
        FRotator Rotation = Hit.Normal.Rotation();

        // 충돌 시 HitEffect 이펙트 스폰
        if (HitEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                HitEffect,
                Hit.Location,
                Rotation
            );
        }

        // PointLight의 강도 0으로 설정
        if (ProjectileLight)
        {
            ProjectileLight->SetIntensity(0.0f);
        }

        DestroySkill();
        
    }
}

void AProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                     bool bFromSweep, const FHitResult & SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == Shooter)
    {
        return;
    }
    
    // 보호막 액터와의 충돌만 처리
    class AShieldActor* Shield = Cast<AShieldActor>(OtherActor);

    
    
    if (Shield)
    {
        // 만약 투사체의 Shooter와 보호막의 소유자(ShieldOwner)가 같다면(내 보호막) 충돌 무시
        if (Shield->ShieldOwner == Shooter)
        {
            //CollisionSphere->IgnoreActorWhenMoving(OtherActor, true);
            return;
        }
        else
        {
            // 다른 플레이어의 보호막과 충돌한 경우 처리
            if (HitEffect)
            {
                FRotator Rotation = SweepResult.Normal.Rotation();
                UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, SweepResult.Location, Rotation);
            }
            DestroySkill();
        }
    }
}

void AProjectileBase::DestroySkill()
{
    if (ProjectileLight)
    {
        ProjectileLight->SetIntensity(0.0f);
        ProjectileLight->Deactivate();
    }

    if (ProjectileEffectComponent)
    {
        ProjectileEffectComponent->Deactivate();
    }

    // Collision 컴포넌트는 더 이상 충돌을 발생시키지 않도록 비활성화
    if (CollisionSphere)
    {
        CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    
    Destroy();
}
