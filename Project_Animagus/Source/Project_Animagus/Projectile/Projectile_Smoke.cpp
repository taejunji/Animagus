// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Smoke.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AProjectile_Smoke::AProjectile_Smoke()
{
    // 필요에 따라 초기화 작업 추가 가능 (예: 속도, 충돌 설정 등)
}

void AProjectile_Smoke::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                               const FHitResult& Hit)
{
    // OtherActor가 유효하며, 자기 자신 및 발사자(Shooter)가 아닌 경우 처리
    if (OtherActor && OtherActor != this && OtherActor != Shooter)
    {
        // 충돌 시 HitEffect (연막 효과) Niagara 시스템이 할당되어 있다면 스폰
        if (HitEffect)
        {
            // 기본적으로는 충돌 위치 Hit.Location에 Niagara 효과를 스폰하지만,
            // 충돌 노멀의 Z 값이 낮다면(바닥 쪽이면) 일정 오프셋을 더해 위쪽으로 효과를 이동시킴.
            FVector SpawnLocation = Hit.Location;
            // 예: 노멀의 Z 값이 0.7보다 작으면 위로 100cm 오프셋 적용 (값은 상황에 맞게 조정)
            if (Hit.Normal.Z > 0.7f)
            {
                SpawnLocation.Z += 150.0f;
            }
            // 회전은 FRotator::ZeroRotator 사용 (회전 없이 위치에서 생성)
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitEffect, SpawnLocation, FRotator::ZeroRotator);
        }
        
        // 추가적으로 데미지 처리나 다른 효과가 필요하면 여기서 구현 가능

        // 충돌 후 투사체 소멸 (즉시 소멸; 필요 시 타이머를 이용해 딜레이 가능)
        DestroySkill();
    }
}

