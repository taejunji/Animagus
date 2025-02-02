#include "FogController.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/ExponentialHeightFog.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Components/ExponentialHeightFogComponent.h"

AFogController::AFogController()
{
    PrimaryActorTick.bCanEverTick = true;

    // 🔹 보이지 않는 충돌 감지용 Sphere 추가
    FogCollision = CreateDefaultSubobject<USphereComponent>(TEXT("FogCollision"));
    FogCollision->SetupAttachment(RootComponent);
    FogCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    FogCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
    FogCollision->SetSphereRadius(InitialRadius);

    // 🔹 나이아가라 독안개 효과 (에디터에서 직접 설정 필요)
    FogEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FogEffect"));
    FogEffect->SetupAttachment(RootComponent);

    // 🔹 FogComponent 초기화
    FogComponent = nullptr;
}

void AFogController::BeginPlay()
{
    Super::BeginPlay();
    CurrentRadius = InitialRadius;

    AExponentialHeightFog* FogActor = nullptr;

    // 🔹 씬에서 Exponential Height Fog 찾기
    for (TActorIterator<AExponentialHeightFog> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        FogActor = *ActorItr;
        break;
    }

    // 🔹 씬에 Exponential Height Fog가 존재하는 경우
    if (FogActor)
    {
        FogComponent = FogActor->GetComponentByClass<UExponentialHeightFogComponent>();

        if (FogComponent)
        {
            UE_LOG(LogTemp, Warning, TEXT("FogComponent 설정 완료!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("씬에 Exponential Height Fog가 없음! 추가해줘야 함."));
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

        // 🔹 나이아가라 크기 조정 (null 체크 추가)
        if (FogEffect)
        {
            FogEffect->SetWorldScale3D(FVector(CurrentRadius / InitialRadius));
        }

        // 🔹 볼류메트릭 포그 밀도 조정 (null 체크 추가)
        if (FogComponent)
        {
            FogComponent->SetFogDensity(FMath::Clamp(CurrentRadius / InitialRadius, 0.2f, 1.0f));
        }
    }
}

// 🔹 스킬과 충돌했을 때 호출되는 함수
void AFogController::OnSkillOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                                    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        FName SkillType = "Default"; // 기본 효과

        // 스킬의 종류를 태그로 판별
        if (OtherActor->ActorHasTag("Skill_Fire"))
        {
            SkillType = "Fire";
        }
        else if (OtherActor->ActorHasTag("Skill_Ice"))
        {
            SkillType = "Ice";
        }

        UE_LOG(LogTemp, Warning, TEXT("스킬이 독안개와 충돌함! 스킬 타입: %s"), *SkillType.ToString());

        // 스킬 타입에 따라 다른 나이아가라 효과 적용
        CreateEffectAtLocation(SweepResult.ImpactPoint, SkillType);
    }
}

// 🔹 나이아가라를 활용해 충돌 효과 생성
void AFogController::CreateEffectAtLocation(FVector ImpactLocation, FName SkillType)
{
    if (FogEffect)
    {
        UE_LOG(LogTemp, Warning, TEXT("FogEffect가 정상적으로 초기화됨!"));
        FogEffect->SetNiagaraVariableFloat(TEXT("HoleSize"), 500.0f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("FogEffect가 초기화되지 않음!"));
    }
}