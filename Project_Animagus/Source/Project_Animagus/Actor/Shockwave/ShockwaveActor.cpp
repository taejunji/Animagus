#include "ShockwaveActor.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Project_Animagus/Character/BaseCharacter.h"

AShockwaveActor::AShockwaveActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Collision: SphereComponent
    ShockwaveCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ShockwaveCollision"));
    ShockwaveCollision->InitSphereRadius(0.f);  // 초기 반경 0
    ShockwaveCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    ShockwaveCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
    ShockwaveCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    ShockwaveCollision->SetGenerateOverlapEvents(true);


    RootComponent = ShockwaveCollision;

    // Niagara Effect Component
    ShockwaveEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShockwaveEffect"));
    ShockwaveEffect->SetupAttachment(RootComponent);

    // 기본 값 설정
    CurrentRadius = 0.f;
    ExpansionSpeed = 1000.f; // 예: 초당 1000cm 확산
    MaxRadius = 1500.f;      
    Damage = 30.f;
    KnockbackForce = 1500.f;
    bHasAppliedEffect = false;
    Shooter = nullptr;
}

void AShockwaveActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Niagara 효과 초기 설정 (User Parameter로 확산 반경 업데이트)
    if (ShockwaveEffect)
    {
        ShockwaveEffect->SetFloatParameter(FName("ShockwaveRadius"), CurrentRadius);
    }

    UE_LOG(LogTemp, Log, TEXT("ShockwaveActor::BeginPlay() - Shockwave actor spawned. InitialRadius: %f"), CurrentRadius);
}

void AShockwaveActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 확산 반경 증가
    CurrentRadius += ExpansionSpeed * DeltaTime;
    if (CurrentRadius > MaxRadius)
    {
        // 최대 반경에 도달하면 충격파 효과 종료
        UE_LOG(LogTemp, Log, TEXT("ShockwaveActor::Tick() - CurrentRadius reached MaxRadius. Destroying actor."));
        Destroy();
        return;
    }

    // 콜리전 반경 업데이트
    ShockwaveCollision->SetSphereRadius(CurrentRadius);

    // Niagara 효과에 확산 반경 업데이트
    if (ShockwaveEffect)
    {
        ShockwaveEffect->SetFloatParameter(FName("ShockwaveRadius"), CurrentRadius);
    }

    UE_LOG(LogTemp, Log, TEXT("ShockwaveActor::Tick() - CurrentRadius: %f"), CurrentRadius);

    TArray<AActor*> OverlappingActors;
    ShockwaveCollision->GetOverlappingActors(OverlappingActors, ABaseCharacter::StaticClass());
    UE_LOG(LogTemp, Log, TEXT("ShockwaveActor::Tick() - Overlapping Actors Count: %d"), OverlappingActors.Num());

    
    ApplyShockwaveEffects();

}

void AShockwaveActor::ApplyShockwaveEffects()
{
    TArray<AActor*> OverlappingActors;
    ShockwaveCollision->GetOverlappingActors(OverlappingActors, ABaseCharacter::StaticClass());

    UE_LOG(LogTemp, Log, TEXT("ShockwaveActor::ApplyShockwaveEffects() - Overlapping Actors Count: %d"), OverlappingActors.Num());
    
    for (AActor* Actor : OverlappingActors)
    {
        // ABaseCharacter로 캐스팅해서 플레이어인지 확인
        ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(Actor);
        if (!TargetCharacter)
        {
            UE_LOG(LogTemp, Log, TEXT("ShockwaveActor::ApplyShockwaveEffects() - Actor %s is not a BaseCharacter, skipping."), *Actor->GetName());
            continue;
        }

        // Shooter 또는 이미 처리된 액터는 무시
        if (Actor == Shooter || AffectedActors.Contains(Actor))
        {
            UE_LOG(LogTemp, Log, TEXT("ShockwaveActor::ApplyShockwaveEffects() - Skipping actor %s (Shooter or already affected)."), *Actor->GetName());
            continue;
        }
        
        // 데미지 적용 (Owner의 컨트롤러를 통해)
        UGameplayStatics::ApplyDamage(Actor, Damage, (Shooter ? Shooter->GetInstigatorController() : nullptr), this, nullptr);

        // 넉백 효과 적용: 충격파 중심에서 멀어지는 방향으로 밀어내기
        FVector Direction = Actor->GetActorLocation() - GetActorLocation();
        Direction.Z = 0;  // 평면상에서만 적용 (필요에 따라 조정)
        Direction.Normalize();
        FVector Impulse = Direction * KnockbackForce;
        if (ACharacter* Character = Cast<ACharacter>(Actor))
        {
            Character->LaunchCharacter(Impulse, true, true);
        }

        // 이미 효과를 적용한 액터는 중복 적용 방지를 위해 기록
        AffectedActors.Add(Actor);
    }
}

void AShockwaveActor::SetShooter(AActor* InShooter)
{
    Shooter = InShooter;
}
