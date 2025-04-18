#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item_Box_Base.generated.h"

class UStaticMeshComponent;
class UGeometryCollectionComponent;
class UNiagaraSystem;
class ABaseItem;
class AProjectileBase;

UCLASS()
class PROJECT_ANIMAGUS_API AItem_Box_Base : public AActor
{
    GENERATED_BODY()

public:
    AItem_Box_Base();

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 상자의 체력. 초기값: 50
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Box")
    float HP;

    // 아이템 스폰 타입: 0이면 BaseItem, 1이면 BaseItemPlus
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Box")
    int32 SpawnItemType;

    // 정적 메쉬 컴포넌트 (intact 상태)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    // Geometry Collection 컴포넌트 (파괴 상태용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UGeometryCollectionComponent* FracturedComp;

    // 부서짐 시 나이아가라 이펙트 (충돌/파괴 효과)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
    UNiagaraSystem* ImpactNiagaraEffect;

    // 아이템 스폰용 BP 클래스들
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Box|Items")
    TSubclassOf<ABaseItem> BaseItemClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Box|Items")
    TSubclassOf<ABaseItem> BaseItemPlusClass;

    // 충돌 이벤트 핸들러
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
               UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    // 박스 파괴 시 실행 함수
    virtual void BreakBox();

    // 래퍼 함수: 타이머로 호출되어 액터를 Destroy()함
    UFUNCTION()
    void HandleDestroyTimer();

public:
    // 데미지를 적용하는 함수
    void ApplyDamage(float DamageAmount);
};
