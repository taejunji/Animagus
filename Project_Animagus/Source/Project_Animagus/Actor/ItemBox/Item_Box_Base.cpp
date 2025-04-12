#include "Item_Box_Base.h"
#include "Components/StaticMeshComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "../../Projectile/ProjectileBase.h"
#include "../../Item/BaseItem.h"
#include "UObject/ConstructorHelpers.h"

AItem_Box_Base::AItem_Box_Base()
{
    PrimaryActorTick.bCanEverTick = true;

    HP = 50.f;
    SpawnItemType = 0; // 기본값

    // 정적 메쉬 컴포넌트 생성 (intact 상태)
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
    MeshComp->OnComponentHit.AddDynamic(this, &AItem_Box_Base::OnHit);

    // Geometry Collection 컴포넌트 생성 (파괴 상태용)
    FracturedComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("FracturedComp"));
    FracturedComp->SetupAttachment(RootComponent);
    // 초기에는 숨기고, 물리 시뮬레이션 끔
    FracturedComp->SetVisibility(false);
    FracturedComp->SetSimulatePhysics(false);
    FracturedComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ImpactNiagaraEffect = nullptr; // 에디터에서 할당

    // 아이템 BP 클래스들은 에디터에서 할당하거나 ConstructorHelpers로 설정

    // BaseItem BP 로드 (PowerUpItem)
    static ConstructorHelpers::FClassFinder<ABaseItem> BaseItemBP(TEXT("/Game/WorkFolder/Bluprints/Item/MyPowerUpItem")); // 실제 경로로 변경
    if (BaseItemBP.Succeeded())
    {
        BaseItemClass = BaseItemBP.Class;
        UE_LOG(LogTemp, Log, TEXT("Item_Box_Base: Successfully loaded BaseItemClass: %s"), *BaseItemClass->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Item_Box_Base: Failed to load BaseItemClass."));
    }

    // BaseItemPlus BP 로드 (PowerUpPlusItem)
    static ConstructorHelpers::FClassFinder<ABaseItem> BaseItemPlusBP(TEXT("/Game/WorkFolder/Bluprints/Item/MyPowerUpPlusItem")); // 실제 경로로 변경
    if (BaseItemPlusBP.Succeeded())
    {
        BaseItemPlusClass = BaseItemPlusBP.Class;
        UE_LOG(LogTemp, Log, TEXT("Item_Box_Base: Successfully loaded BaseItemPlusClass: %s"), *BaseItemPlusClass->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Item_Box_Base: Failed to load BaseItemPlusClass."));
    }
}

void AItem_Box_Base::BeginPlay()
{
    Super::BeginPlay();
}

void AItem_Box_Base::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AItem_Box_Base::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
                             UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    // 투사체와 충돌 시 데미지 적용
    AProjectileBase* Projectile = Cast<AProjectileBase>(OtherActor);
    if (Projectile)
    {
        float Damage = Projectile->DamageValue; // ProjectileBase의 공개 멤버
        ApplyDamage(Damage);
    }
}

void AItem_Box_Base::ApplyDamage(float DamageAmount)
{
    HP -= DamageAmount;
    UE_LOG(LogTemp, Log, TEXT("Item_Box_Base: HP reduced to: %f"), HP);

    if (ImpactNiagaraEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactNiagaraEffect, GetActorLocation());
    }
    
    if (HP <= 0.f)
    {
        BreakBox();
    }
}

void AItem_Box_Base::BreakBox()
{
    UE_LOG(LogTemp, Log, TEXT("Item_Box_Base: Box is broken."));
    

    // 2. 정적 메쉬를 숨김
    if (MeshComp)
    {
        MeshComp->SetVisibility(false);
        MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // 3. Geometry Collection 컴포넌트를 활성화하여 파괴 효과 적용
    if (FracturedComp)
    {
        FracturedComp->SetVisibility(true);
        FracturedComp->SetSimulatePhysics(true);
        FracturedComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        // 파편들이 부드럽게 분리되도록 임펄스 적용 (튜닝 필요)
        float ImpulseRadius = 100.f;
        float ImpulseStrength = 400.f;  // 적절히 튜닝
        FracturedComp->AddRadialImpulse(GetActorLocation(), ImpulseRadius, ImpulseStrength, ERadialImpulseFalloff::RIF_Linear, true);
    }

    // 4. 아이템 스폰 처리 (SpawnItemType에 따라 BaseItemClass 또는 BaseItemPlusClass)
    TSubclassOf<ABaseItem> ItemToSpawn = (SpawnItemType == 0) ? BaseItemClass : BaseItemPlusClass;
    if (ItemToSpawn)
    {
        FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;
        FActorSpawnParameters SpawnParams;
        ABaseItem* SpawnedItem = GetWorld()->SpawnActor<ABaseItem>(ItemToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
        if (SpawnedItem)
        {
            UE_LOG(LogTemp, Log, TEXT("Item_Box_Base: Spawned item at %s"), *SpawnLocation.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Item_Box_Base: Failed to spawn item."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Item_Box_Base: No item class assigned for spawn."));
    }

    // 5. 액터를 일정 시간 후 제거하도록 타이머 설정 
    FTimerHandle TempHandle;
    GetWorld()->GetTimerManager().SetTimer(TempHandle, this, &AItem_Box_Base::HandleDestroyTimer, 20.0f, false);
}

void AItem_Box_Base::HandleDestroyTimer()
{
    Destroy();
}
