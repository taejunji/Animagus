#include "ShockwaveSkill.h"
#include "Project_Animagus/Actor/Shockwave/ShockwaveActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"

UShockwaveSkill::UShockwaveSkill()
{
    SkillName = "ShockwaveSkill";
    SkillDescription = "Emits a shockwave that pushes away nearby players and applies damage.";
    
    // 스킬 쿨타임 등은 필요에 따라 에디터에서 조정 가능
    CooldownTime = 10.f;
    bIsCooldown = false;
    LastUsedTime = -CooldownTime;
    bFirstUse = true;
    
    // 충격파 스킬 관련 기본 파라미터
    ShockwaveDamage = 10.f;
    KnockbackForce = 1500.f;
    ExpansionSpeed = 300.f;  // cm/s
    MaxRadius = 300.f;       // cm

    BaseCooldownTime = CooldownTime;
    BaseKnockbackForce = KnockbackForce;
    
    //ShockwaveActorBPClass = nullptr; // 에디터에서 할당 (충격파 액터 블루프린트)
    static ConstructorHelpers::FClassFinder<AShockwaveActor> ShockwaveBPFinder(TEXT("/Game/WorkFolder/Bluprints/Shockwave/MyShockwaveActor"));
    if (ShockwaveBPFinder.Succeeded())
    {
        ShockwaveActorBPClass = ShockwaveBPFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Fireball BP class!"));
    }

    SkillType = Protocol::SkillType::SHOCKWAVE;

}

void UShockwaveSkill::ActiveSkill_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("UShockwaveSkill::ActiveSkill_Implementation() called on %s"), *GetName());

    // 스킬 활성화 조건 체크 (Owner 존재 및 쿨타임 여부)
    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("UShockwaveSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
        return;
    }
    
    UWorld* World = Owner->GetWorld();
    if (!World || !ShockwaveActorBPClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("UShockwaveSkill: Cannot activate - World or ShockwaveActorBPClass is null."));
        return;
    }
    
    // 공격 애니메이션
    Owner->PlayAnimMontageByType(MontageType::DefaultAttack);

    // 스폰 위치는 스킬 시전자의 현재 위치
    FVector SpawnLocation = Owner->GetActorLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;

    // ShockwaveActor 스폰
    AShockwaveActor* ShockwaveActor = World->SpawnActor<AShockwaveActor>(ShockwaveActorBPClass, SpawnLocation - FVector(0.f,20.f,0.f), SpawnRotation);
    if (ShockwaveActor)
    {
        // 스폰된 ShockwaveActor에 스킬 관련 파라미터 전달
        ShockwaveActor->SetShooter(Owner);
        ShockwaveActor->Damage = ShockwaveDamage;
        ShockwaveActor->KnockbackForce = KnockbackForce;
        ShockwaveActor->ExpansionSpeed = ExpansionSpeed;
        ShockwaveActor->MaxRadius = MaxRadius;

        UE_LOG(LogTemp, Log, TEXT("UShockwaveSkill: ShockwaveActor spawned successfully at location: %s"), *SpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UShockwaveSkill: Failed to spawn ShockwaveActor."));
    }
    
    // 첫 사용이면 플래그 변경 후 쿨타임 시작
    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    StartCooldown();
}

void UShockwaveSkill::UpgradeSkill(int32 NewPowerUpLevel)
{
   
    float CooldownMultiplier = FMath::Clamp(1.0f - (0.05f * NewPowerUpLevel), 0.5f, 1.0f);
    
    CooldownTime = BaseCooldownTime * CooldownMultiplier;

}