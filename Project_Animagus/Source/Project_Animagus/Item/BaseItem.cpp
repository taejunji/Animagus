#include "BaseItem.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Character/BaseCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

ABaseItem::ABaseItem()
{
    PrimaryActorTick.bCanEverTick = true;

    // Collision 컴포넌트 생성 및 초기화
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComp->InitSphereRadius(50.f);
    CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

    // 모든 채널은 무시하도록 설정한 뒤
    CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    // Pawn 채널만 Overlap 처리
    CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
    
    CollisionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
    CollisionComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 추가 
    CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // 오버랩 추가

    CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); 

    RootComponent = CollisionComp;

    // 충돌 이벤트 바인딩: OnItemOverlapBegin 함수 호출
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnItemOverlapBegin);
    
    // 시야 시스템에 인식하도록
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource")); 

    // Niagara 컴포넌트 생성 (상시 이펙트)
    ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffect"));
    ItemEffect->SetupAttachment(RootComponent);

    bIsPickedUp = false;
}

void ABaseItem::BeginPlay()
{
    Super::BeginPlay();

    // 아이템 스폰 시 SpawnEffect가 할당되어 있으면 재생
    if (SpawnEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnEffect, GetActorLocation(), GetActorRotation());
    }
    
    // 상시 이펙트 활성화 (Asset이 할당되어 있다면)
    if (ItemEffect && ItemEffect->GetAsset())
    {
        ItemEffect->Activate();
    }

    // 시야 시스템 감지 등록
    if (StimuliSource)
    {
        StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());  // 시각 감지에 등록
        StimuliSource->RegisterWithPerceptionSystem();  // 감지 시스템에 등록
    }
}

void ABaseItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABaseItem::OnItemOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                   bool bFromSweep, const FHitResult & SweepResult)
{
    // 기본 구현: OtherActor가 ABaseCharacter이면 OnPickedUp 호출
    if (!bIsPickedUp && OtherActor && OtherActor != this)
    {
        ABaseCharacter* Character = Cast<ABaseCharacter>(OtherActor);
        if (Character)
        {
            if (PickSound)
            {
                UGameplayStatics::PlaySoundAtLocation(
                    this,
                    PickSound,
                    this->GetActorLocation(),
                    FRotator::ZeroRotator,
                    1.f, 1.f, 0.f,
                    AttenuationSettings
                );
            } 
            OnPickedUp(Character);
        }
    }
}

void ABaseItem::OnPickedUp(ABaseCharacter* Picker)
{
    if (!bIsPickedUp)
    {
        bIsPickedUp = true;
       
        DestroyItem();
    }
}

void ABaseItem::DestroyItem()
{

    // 아이템 획득 시 PickupEffect가 할당되어 있으면 재생
    if (PickupEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupEffect, GetActorLocation(), GetActorRotation());
    }

    
    if (ItemEffect)
    {
        ItemEffect->Deactivate();
    }
    Destroy();
}


