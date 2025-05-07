// HasteSkill.cpp

#include "HasteSkill.h"
#include "../Character/BaseCharacter.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

UHasteSkill::UHasteSkill()
{
    SkillName        = "Haste";
    SkillDescription = TEXT("일정 시간 동안 이동 속도가 증가하고 이펙트 및 포스트프로세싱을 활성화합니다.");
    Duration         = 5.0f;
    BoostedRunSpeed  = 1000.0f;
    CooldownTime     = 20.0f;
    PostProcessSpeedThreshold = 500.0f;

    BaseBoostedRunSpeed = BoostedRunSpeed;
    BaseHasteColldown = CooldownTime;
    
    SkillType = Protocol::SkillType::HASTE;
}

void UHasteSkill::ActiveSkill_Implementation()
{
    if (!CanActivateSkill() || Owner == nullptr) return;

    ABaseCharacter* Char = Cast<ABaseCharacter>(Owner);
    if (Char)
    {
        // 속도 증가
        OriginalRunSpeed            = Char->default_run_speed;
        Char->default_run_speed     = BoostedRunSpeed;
        Char->SetWalkSpeed(BoostedRunSpeed);

        if (LanchedSound && Char->IsLocallyControlled() && Char->GetPawnType() == PawnType::PLAYER)
        {
            UGameplayStatics::PlaySound2D(this, LanchedSound);
        }
        
        // Niagara 이펙트 켜기
        if (Char->SpeedBoostComponent)
            Char->SpeedBoostComponent->Activate();

        if (UWorld* W = Char->GetWorld())
        {
            W->GetTimerManager().SetTimer(
                SpeedCheckTimerHandle,
                this,
                &UHasteSkill::CheckSpeed,
                0.1f,
                true
            );
        }

        // Duration 후 비활성화 예약
        if (UWorld* W = Char->GetWorld())
        {
            W->GetTimerManager().SetTimer(
                HasteTimerHandle,
                this,
                &UHasteSkill::DeactiveSkill,
                Duration,
                false
            );
        }
    }

    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    StartCooldown();
}

void UHasteSkill::DeactiveSkill_Implementation()
{
    if (Owner == nullptr) return;

    ABaseCharacter* Char = Cast<ABaseCharacter>(Owner);
    if (Char)
    {
        // 속도 원복
        Char->default_run_speed = OriginalRunSpeed;
        Char->SetWalkSpeed(OriginalRunSpeed);

        // Niagara 이펙트 끄기
        if (Char->SpeedBoostComponent)
            Char->SpeedBoostComponent->Deactivate();

        // 3) 반복 검사 멈추기
        if (UWorld* W = Char->GetWorld())
        {
            W->GetTimerManager().ClearTimer(SpeedCheckTimerHandle);
        }
        
        // PostProcessVolume 가중치 0으로 리셋
        SetHastePostProcess(0.0f);
    }
}

void UHasteSkill::CheckSpeed()
{
    ABaseCharacter* Char = Cast<ABaseCharacter>(Owner);
    if (!Char) return;

    // 로컬 플레이어 캐릭터인지 확인
    AController* Ctrl = Char->GetController();
    if (Ctrl == nullptr) return;
    if (Char->GetPawnType() != PawnType::PLAYER) return;


    // current_speed 기준으로 PostProcess 토글
    if (Char->current_speed >= PostProcessSpeedThreshold)
    {
        SetHastePostProcess(1.0f);
    }
    else
    {
        SetHastePostProcess(0.0f);
    }
}

void UHasteSkill::SetHastePostProcess(float NewWeight)
{
    UWorld* World = nullptr;
    if (Owner)
        World = Owner->GetWorld();
    if (!World) return;

    TArray<AActor*> Volumes;
    UGameplayStatics::GetAllActorsOfClass(World, APostProcessVolume::StaticClass(), Volumes);

    for (AActor* A : Volumes)
    {
        APostProcessVolume* PPV = Cast<APostProcessVolume>(A);
        if (PPV)
        {
            // 예: 배열 인덱스 3 (4번째 블렌더)
            const int32 Index = 3;
            if (PPV->Settings.WeightedBlendables.Array.Num() > Index)
            {
                PPV->Settings.WeightedBlendables.Array[Index].Weight = NewWeight;
            }
        }
    }
}

void UHasteSkill::UpgradeSkill(int32 NewPowerUpLevel)
{
   
    int32 Level = FMath::Clamp(NewPowerUpLevel, 0, 14);
    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);
    CooldownTime = BaseHasteColldown * CooldownMultiplier;

    BoostedRunSpeed = BaseBoostedRunSpeed + (Level * 100.f);
    

    UE_LOG(LogTemp, Log, TEXT("BoostedRunSpeed upgraded: Level %d, BoostedRunSpeed: %f, CooldownTime: %f"), Level, BoostedRunSpeed, CooldownTime);
}
