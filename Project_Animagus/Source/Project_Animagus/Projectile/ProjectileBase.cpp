#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AProjectileBase::AProjectileBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 충돌체 생성 및 초기화
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->InitSphereRadius(10.0f);
   // 추후에 Projectile로 설정 후 충돌채널 설정 
    //CollisionSphere->SetCollisionProfileName(TEXT("BlockAll"));
    RootComponent = CollisionSphere;
   // CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    
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

    // 충돌 이벤트 바인딩
    CollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}

void AProjectileBase::BeginPlay()
{
    Super::BeginPlay();

    // 스폰 시 FlashEffect 이펙트 재생
    if (FlashEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            FlashEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }

    // Lifetime 타이머 설정: Lifetime 후에 DestroySkill 호출
    FTimerHandle LifetimeHandle;
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

        // 충돌 후 1초 후 DestroySkill 호출 (혹은 즉시 Destroy()를 호출할 수도 있음)
        FTimerHandle DestroyHandle;
        GetWorld()->GetTimerManager().SetTimer(
            DestroyHandle,
            this,
            &AProjectileBase::DestroySkill,
            1.0f,
            false
        );
    }
}

void AProjectileBase::DestroySkill()
{
    Destroy();
}
