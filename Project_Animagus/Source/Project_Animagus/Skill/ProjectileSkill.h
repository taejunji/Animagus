// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "ProjectileSkill.generated.h"

/**
 *  에디터에서 설정할거는 point_light, projectile_effect, flash_effect, hit_effect
 */
UCLASS()
class PROJECT_ANIMAGUS_API AProjectileSkill : public ABaseSkill
{
	GENERATED_BODY()

public:
    AProjectileSkill(); 

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // 컴포넌트 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    class USphereComponent* collision_component;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    class UProjectileMovementComponent* projectile_movement;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light")
    class UPointLightComponent* point_light;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
    class UNiagaraComponent* projectile_effect; // 투사체 따라가는 이펙트 -> 지속효과

    // 변수
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
    class UNiagaraSystem* flash_effect; // 처음 생성 이펙트

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
    class UNiagaraSystem* hit_effect; // 피격 이펙트

public:

    // 투사체 속도 설정
    void SetInitialSpeed(float InitalSpeed, float MaxSpeed);
    
    // 충돌 처리 함수 -> Block이 되었을 때 호출
    // 1. OverlappedComponent: 투사체의 충돌 컴포넌트
    // 2. AActor: 충돌한 다른 액터
    // 3. OtherComp: 충돌한 다른 액터의 컴포넌트
    // 4. NormalImpulse: 충돌 시 다른 객체에 가해진 힘의 크기와 방향
    // 5. 충돌에 대한 상세 정보를 포함하는 구조체입니다. 이 구조체는 충돌 지점, 법선 벡터, 충돌 깊이 등 다양한 정보
    UFUNCTION()
    void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // * 스킬 사용 함수
    virtual void ActivateSkill(AActor* Target);  // 스킬 활성화

    void DestroySkill();
};
