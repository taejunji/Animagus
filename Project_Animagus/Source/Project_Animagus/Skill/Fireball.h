#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "Fireball.generated.h"

/**
 * Fireball 스킬 클래스
 * UBaseSkill을 상속받아 파이어볼 스킬에 특화된 기본 정보만 관리합니다.
 * 실제 시각적 효과(Flash, Hit 효과 등)는 투사체 액터(Projectile_FireBall) 블루프린트에서 설정합니다.
 */
UCLASS(Blueprintable)
class PROJECT_ANIMAGUS_API UFireball : public UBaseSkill
{
    GENERATED_BODY()

public:
    UFireball();

    /** 파이어볼 데미지 값 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fireball|Damage")
    float FireballDamage;

    /** 파이어볼 이동 속도 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fireball|Movement")
    float FireballSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fireball")
    TSubclassOf<class AProjectile_FireBall> ProjectileBPClass;
    
    // ActiveSkill 인터페이스 오버라이드: 스킬 사용 시 호출
    virtual void ActiveSkill_Implementation() override;
};
