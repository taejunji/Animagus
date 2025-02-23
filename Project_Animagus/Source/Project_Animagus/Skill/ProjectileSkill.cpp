// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileSkill.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/PointLightComponent.h"

AProjectileSkill::AProjectileSkill()
{
    PrimaryActorTick.bCanEverTick = true;
 
    // 충돌 컴포넌트 초기화
    collision_component = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = collision_component;
    collision_component->InitSphereRadius(10.f);
    // 충돌 이벤트 바인딩
    collision_component->OnComponentHit.AddDynamic(this, &AProjectileSkill::OnHit);

    // 충돌 채널 - 에디터

    // 투사체 이동 컴포넌트 초기화
    projectile_movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    projectile_movement->InitialSpeed = 2500.f;
    projectile_movement->MaxSpeed = 3000.f;
    projectile_movement->ProjectileGravityScale = 0.1f; 

    projectile_effect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    projectile_effect->SetupAttachment(collision_component);
    projectile_effect->bAutoActivate = true; // 자동 실행 활성화

    // [투사체 에셋] 1. 코드에서 설정하기 2. 에디터에서 설정하기
    //static ConstructorHelpers::FObjectFinder<UNiagaraSystem> projectile_trail_effect(TEXT("/Game/ToonProjectiles/Effects/P_Projectile_7.P_Projectile_7"));
    //if (projectile_trail_effect.Succeeded())
    //{
    //    projectile_effect->SetAsset(projectile_trail_effect.Object);
    //}

    // 조명
    point_light = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
    point_light->SetupAttachment(collision_component);
    point_light->LightColor = FColor(255, 169, 0);

    life_time = 10.f; // 지속시간

}

void AProjectileSkill::BeginPlay()
{
    Super::BeginPlay();

    FVector SpawnLocation = GetActorLocation();
    FRotator SpawnRotation = GetActorRotation();
    if (flash_effect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), flash_effect, SpawnLocation, SpawnRotation);

        FTimerHandle LifeTime;
        GetWorld()->GetTimerManager().SetTimer(LifeTime, this, &AProjectileSkill::DestroySkill, life_time, false);
    }
}

void AProjectileSkill::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AProjectileSkill::SetInitialSpeed(float InitalSpeed, float MaxSpeed)
{
    if (projectile_movement)
    {
        projectile_movement->InitialSpeed = InitalSpeed;
        projectile_movement->MaxSpeed = MaxSpeed;
    }
}

void AProjectileSkill::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        // 충돌체의 법선 벡터 -> 충돌한 표면의 방향 -> 충돌 지점에서 수직으로 나오는 벡터
        FVector ImpactNormal = Hit.Normal;

        // 월드 기준으로 나타낸 방향 벡터인 ImpactNormal을 월드 기준의 회전값(FRotator)으로 변환
        FRotator Rotation = ImpactNormal.Rotation();

        // 이펙트를 벽이 바깥을 바라보는쪽으로 생성하는 역할
        if (hit_effect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), hit_effect, Hit.Location, Rotation);
        }

        point_light->SetIntensity(0.0f);

        FTimerHandle LifeTime;
        GetWorld()->GetTimerManager().SetTimer(LifeTime, this, &AProjectileSkill::DestroySkill, 1.0f, false);
    }
}

void AProjectileSkill::ActivateSkill(AActor* Target)
{
    AActor* MyOwner = GetOwner();
    FVector Direction = MyOwner->GetActorForwardVector(); // 앞 방향을 가져옴
    projectile_movement->Velocity = Direction * projectile_movement->InitialSpeed;
    
}

void AProjectileSkill::DestroySkill()
{
    // 액터 삭제
    Destroy();
}