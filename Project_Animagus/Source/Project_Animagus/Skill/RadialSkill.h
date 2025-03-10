#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"
#include "RadialSkill.generated.h"

/**
 * URadialSkill
 * 방사형 스킬 클래스.
 * 스킬 사용 시 캐릭터의 위치를 기준으로 부채꼴 모양으로 여러 투사체를 한 번에 생성한다.
 */
UCLASS(Blueprintable)
class PROJECT_ANIMAGUS_API URadialSkill : public UBaseSkill
{
    GENERATED_BODY()

public:
    URadialSkill();

    // 발사할 투사체 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radial|Parameters")
    int32 NumberOfProjectiles;

    // 전체 부채꼴 각도 (도 단위)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radial|Parameters")
    float SpreadAngle;

    // 스폰할 투사체 블루프린트 클래스 (AProjectileBase를 상속받은 클래스)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radial")
    TSubclassOf<class AProjectileBase> ProjectileBPClass;

    // 각 투사체에 적용할 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radial|Damage")
    float RadialDamage;

    // 각 투사체의 이동 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radial|Movement")
    float RadialSpeed;

    // 투사체를 캐릭터 중심에서 떨어뜨릴 수평 거리 (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radial|Parameters")
    float SpawnRadius;

    // 투사체를 캐릭터 기준으로 위쪽으로 올릴 높이 (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radial|Parameters")
    float VerticalOffset;

protected:
    // 스킬 사용 시 호출되는 함수
    virtual void ActiveSkill_Implementation() override;
};
