#include "Projectile_MagicMissile.h"

#include "../Character/BaseCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Components/PointLightComponent.h"

AProjectile_MagicMissile::AProjectile_MagicMissile()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // 초기 가속 관련 기본값 설정
    AccelerationThreshold = 0.7f;         // 생성 후 3초가 지나면 가속 시작
    SuddenAccelerationFactor = 2.f;        // 이 값은 원하는 가속의 강도에 맞게 조정 (높을수록 급격하게 가속)
    
    ProjectileMovement->InitialSpeed = 500.f;
    // ProjectileMovementComponent가 ProjectileBase에 이미 포함되어 있다고 가정합니다.
    if (ProjectileMovement)
    {
        ProjectileMovement->MaxSpeed = 5000.f;
        ProjectileMovement->bIsHomingProjectile = false;
    }
}

void AProjectile_MagicMissile::BeginPlay()
{
    Super::BeginPlay();
    // 투사체 생성 시점을 기록합니다.
    SpawnTime = GetWorld()->GetTimeSeconds();
}

void AProjectile_MagicMissile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ProjectileMovement)
    {
        float ElapsedTime = GetWorld()->GetTimeSeconds() - SpawnTime;
        FVector CurrentVelocity = ProjectileMovement->Velocity;
        float CurrentSpeed = CurrentVelocity.Size();
        float NewSpeed = CurrentSpeed;

        // 일정 시간이 지나면 갑자기 가속: 예를 들어, 3초 이후부터 SuddenAccelerationFactor에 따라 속도가 급증
        if (ElapsedTime >= AccelerationThreshold)
        {
            // 가속 적용: NewSpeed를 현재 속도에 SuddenAccelerationFactor * DeltaTime를 더하는 대신,
            // 보다 극적인 증가를 위해 현재 속도에 비례하는 곱셈 계수를 사용합니다.
            float Multiplier = 1.0f + SuddenAccelerationFactor * DeltaTime;
            NewSpeed = CurrentSpeed * Multiplier;
        }
        // 속도 방향은 그대로 유지하며, 새 속도를 적용
        FVector NewVelocity = CurrentVelocity.GetSafeNormal() * NewSpeed;
        ProjectileMovement->Velocity = NewVelocity;

        UE_LOG(LogTemp, Log, TEXT("Projectile_MagicMissile: ElapsedTime: %f, NewSpeed: %f"), ElapsedTime, NewSpeed);
    }
}

void AProjectile_MagicMissile::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                      const FHitResult& Hit)
{


    
    if (OtherActor == Shooter )
    {
        return;
        // ProjectileLight->SetIntensity(0.0f);
        // DestroySkill();
    }
    
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {
        // 적용할 데미지: UGameplayStatics::ApplyDamage
        UGameplayStatics::ApplyDamage(OtherActor, DamageValue, GetInstigatorController(), this, nullptr);

        // PointLight 강도 0으로 감소
        if (ProjectileLight)
        {
            ProjectileLight->SetIntensity(0.0f);
        }

        // 카메라 쉐이크, 플레이어와 거리가 멀면 쉐이크 안되도록
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController && HitCameraShakeClass)
        {
            if (ACharacter* Player = Cast<ACharacter>(PlayerController->GetPawn()))
            {
                // 플레이어와 투사체 거리 계산
                float DistanceToPlayer = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
                if (DistanceToPlayer < MaxShakeDistance) // ex) 10m 안에서만 쉐이크
                {
                    PlayerController->ClientStartCameraShake(HitCameraShakeClass);
                }
            }
        }

        if (ACharacter* HitCharacter = Cast<ACharacter>(OtherActor))
        {
            // 충돌 표면의 법선이 대상에게 밀리는 방향이므로 이를 이용
            FVector ImpulseDirection = -Hit.Normal;
            // 필요한 경우, 추가 보정: 예를 들어 대상의 뒤쪽으로 밀리게 하려면 -Hit.Normal을 사용할 수도 있음.
            // 여기서는 충돌 표면의 외부 방향으로 밀어내는 효과를 줍니다.

            float MinZ = 0.5f;
            if (ImpulseDirection.Z < MinZ)
            {
                ImpulseDirection.Z = MinZ;
                ImpulseDirection.Normalize();  // 보정 후 재정규화
            }
            
            FVector LaunchVelocity = ImpulseDirection * KnockbackForce;
            HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
            
        }
        
        
        //  추가 이펙트: 예를 들어 HitEffect를 스폰
        if (HitEffect)
        {
            FRotator Rotation = Hit.Normal.Rotation();
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, Hit.Location, Rotation);
        }

        // 즉시 투사체 소멸
        DestroySkill();
    }
}


