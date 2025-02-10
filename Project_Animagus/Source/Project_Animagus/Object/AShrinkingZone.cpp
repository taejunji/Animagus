// Fill out your copyright notice in the Description page of Project Settings.


#include "AShrinkingZone.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Developer/AITestSuite/Public/AITestsCommon.h"

AAShrinkingZone::AAShrinkingZone()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsShrinking = false;
    CurrentSafeRadius = InitialSafeRadius;
}

void AAShrinkingZone::BeginPlay()
{
    Super::BeginPlay();
    
    LevelStartTime = FAITestHelpers::GetWorld()->GetTimeSeconds();
}

void AAShrinkingZone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float ElapsedTime = FAITestHelpers::GetWorld()->GetTimeSeconds() - LevelStartTime;
    
    // 안전구역 축소 시작 조건 확인
    if (ElapsedTime >= ShrinkStartTime)
    {
        bIsShrinking = true;
        
        // 축소 진행률 계산 (0.0 ~ 1.0)
        float ShrinkElapsed = ElapsedTime - ShrinkStartTime;
        float Alpha = FMath::Clamp(ShrinkElapsed / ShrinkDuration, 0.0f, 1.0f);
        
        // 안전구역 반지름 선형 보간: 처음은 InitialSafeRadius, 최종은 0.0
        CurrentSafeRadius = FMath::Lerp(InitialSafeRadius, 0.0f, Alpha);
        
        // (조건 3) 안전구역 외 플레이어에게 피해 적용 (플레이어 캐릭터 구조 미확정으로 주석 처리)
//         TArray<AActor*> FoundPlayers;
//         UGameplayStatics::GetAllActorsOfClass(GetWorld(), AYourGameCharacter::StaticClass(), FoundPlayers);
//         for (AActor* Actor : FoundPlayers)
//         {
//             AYourGameCharacter* PlayerChar = Cast<AYourGameCharacter>(Actor);
//             if (PlayerChar)
//             {
//                 FVector PlayerLocation = PlayerChar->GetActorLocation();
//                 // 안전구역의 중심을 현재 액터 위치로 가정 (필요에 따라 수정)
//                 float DistanceFromCenter = FVector::Dist(PlayerLocation, GetActorLocation());
//                 if (DistanceFromCenter > CurrentSafeRadius)
//                 {
//                     // 플레이어가 안전구역 외에 있다면 초당 DamagePerSecond만큼 피해 적용
//                     UGameplayStatics::ApplyDamage(PlayerChar, DamagePerSecond * DeltaTime, nullptr, this, nullptr);
//                 }
//             }
//         }
    }
    
    // 디버그용으로 현재 안전구역 반지름을 녹색 구체로 표시 (매 Tick마다 업데이트)
    DrawDebugSphere(GetWorld(), GetActorLocation(), CurrentSafeRadius, 32, FColor::Green, false, DeltaTime, 0, 2.0f);
}

// (필요한 경우) 안전구역 외 플레이어에게 피해 적용 함수 (현재 Tick 내에서 직접 처리하므로 별도 함수로 분리하지 않음)
// void AShrinkingZone::ApplyDamageOutsideSafeZone(float DeltaTime)
// {
//     // 플레이어를 검색하여 안전구역 밖에 있을 경우 피해 적용하는 로직 구현
// }

