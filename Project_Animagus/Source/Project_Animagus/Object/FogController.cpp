#include "FogController.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AFogController::AFogController()
{
    PrimaryActorTick.bCanEverTick = true;

    // 🔹 충돌 감지용 Sphere 설정
    FogCollision = CreateDefaultSubobject<USphereComponent>(TEXT("FogCollision"));
    FogCollision->SetupAttachment(RootComponent);
    FogCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    FogCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
    FogCollision->SetSphereRadius(InitialRadius);

    // 🔹 나이아가라 Fog 효과 (Niagara 시스템은 에디터에서 직접 설정해야 함)
    FogEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FogEffect"));
    FogEffect->SetupAttachment(RootComponent);
}

void AFogController::BeginPlay()
{
    Super::BeginPlay();
    CurrentRadius = InitialRadius;

    // 🔹 나이아가라 시스템 활성화
    if (FogEffect)
    {
        FogEffect->Activate();
        FogEffect->SetWorldScale3D(FVector(InitialRadius / 100.0f));
    }
}

void AFogController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentRadius > MinRadius)
    {
        CurrentRadius -= ShrinkRate * DeltaTime;

        // 🔹 충돌 감지용 Sphere 크기 업데이트
        FogCollision->SetSphereRadius(CurrentRadius);

        // 🔹 Niagara Fog 크기 업데이트 (점점 줄어드는 효과)
        if (FogEffect)
        {
            FogEffect->SetNiagaraVariableFloat(TEXT("User.FogRadius"), CurrentRadius);
            FogEffect->SetWorldScale3D(FVector(CurrentRadius / 100.0f));

            // 🔹 Fog를 중앙으로 이동
            FVector FogLocation = FogEffect->GetComponentLocation();
            FogLocation = FMath::VInterpTo(FogLocation, FVector(0, 0, FogLocation.Z), DeltaTime, 0.3f);
            FogEffect->SetWorldLocation(FogLocation);
        }
    }
}
