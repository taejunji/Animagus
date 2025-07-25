#include "RadialSkill.h"
#include "../Projectile/ProjectileBase.h"
#include "../Projectile/Projectile_Radial.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../AI/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

URadialSkill::URadialSkill()
{
    SkillName = "RadialSkill";           // 스킬 이름 설정
    CooldownTime = 8.0f;                 // 쿨타임 8초
    NumberOfProjectiles = 3;             // 7개의 투사체 발사
    SpreadAngle = 190.0f;                 // 전체 90도 부채꼴
    RadialDamage = 10.0f;                // 투사체 데미지
    RadialSpeed = 1600.f;                // 투사체 속도
    SkillDescription = TEXT("여러개의 투사체를 발사합니다.");
    SpawnRadius = 80.f;                  // 캐릭터 기준으로 80cm 떨어진 위치에서 스폰
    VerticalOffset = 20.f;               // 캐릭터 기준으로 위쪽 20cm

    BaseNumberOfProjectiles = NumberOfProjectiles;
    BaseRadialDamage = RadialDamage;
    
    //ProjectileBPClass = nullptr;         // 에디터에서 할당 필요
    static ConstructorHelpers::FClassFinder<AProjectile_Radial> RadialBPFinder(TEXT("/Game/WorkFolder/Bluprints/Projectiles/MyProjectile_Radial"));
    if (RadialBPFinder.Succeeded())
    {
        ProjectileBPClass = RadialBPFinder.Class;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load Radial BP class!"));
    }


    SkillType = Protocol::SkillType::RADIAL;
}

void URadialSkill::ActiveSkill_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("URadialSkill::ActiveSkill_Implementation() 호출됨."));

    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
        return;
    }
    
    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("URadialSkill: World가 null임."));
        return;
    }
    
    // 공격 애니메이션
    Owner->PlayAnimMontageByType(MontageType::DefaultAttack);

    FVector CharacterLocation = Owner->GetActorLocation();
    FRotator BaseRotation;

    FVector CameraLocation;
    FRotator CameraRotation;
    if (Owner->GetPawnType() == PawnType::PLAYER)
    {
        Owner->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
        BaseRotation = CameraRotation;
    }
    else
    {
        BaseRotation = Rotation;
    }

    // --- 회전 스프레드 계산
    float BaseYaw = BaseRotation.Yaw;
    float AngleStep = (NumberOfProjectiles > 1) ? SpreadAngle / (NumberOfProjectiles - 1) : 0.f;
    float StartYaw = BaseYaw - (SpreadAngle * 0.5f);

    UE_LOG(LogTemp, Log, TEXT("URadialSkill: BaseYaw = %f, AngleStep = %f, StartYaw = %f"), BaseYaw, AngleStep, StartYaw);

    // --- 투사체 생성 루프
    for (int32 i = 0; i < NumberOfProjectiles; ++i)
    {
        float CurrentYaw = StartYaw + i * AngleStep;

        // 수평 스폰 위치: 캐릭터 위치에서 현재 각도에 따른 단위 벡터에 SpawnRadius를 곱함
        FRotator HorizontalRot(0.f, CurrentYaw, 0.f); 
        FVector SpawnLocation = CharacterLocation + HorizontalRot.Vector() * SpawnRadius + FVector(0.f, 0.f, VerticalOffset); 
        FRotator SpawnRotation = FRotator(BaseRotation.Pitch, CurrentYaw, BaseRotation.Roll);

        UE_LOG(LogTemp, Log, TEXT("URadialSkill: %d번째 투사체 스폰, SpawnLocation = %s, SpawnRotation = %s"), i, *SpawnLocation.ToString(), *SpawnRotation.ToString());

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = Owner;
        SpawnParams.Instigator = Owner->GetInstigator();

        // 투사체 액터 스폰: 이때 SpawnParams를 전달하면, 액터가 생성되는 순간부터 Owner와 Instigator 값이 설정됨
        AProjectileBase* Projectile = World->SpawnActor<AProjectileBase>(
            ProjectileBPClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );

        if (ProjectileBPClass)
        {
            if (Projectile)
            {
                Projectile->Shooter = Owner;
                Projectile->DamageValue = RadialDamage;

                if (Projectile->ProjectileMovement)
                {
                    Projectile->ProjectileMovement->InitialSpeed = RadialSpeed;
                    Projectile->ProjectileMovement->MaxSpeed = RadialSpeed;
                }

                UE_LOG(LogTemp, Log, TEXT("URadialSkill: %d번째 투사체 스폰 성공: %s"), i, *Projectile->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("URadialSkill: %d번째 투사체 스폰 실패"), i);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("URadialSkill: ProjectileBPClass 할당되지 않음."));
            break;
        }
    }

    // 첫 사용이면 플래그 변경
    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    StartCooldown();
}

void URadialSkill::UpgradeSkill(int32 NewPowerUpLevel)
{
    int32 Level = FMath::Clamp(NewPowerUpLevel, 0, 14);
    
    int32 ExtraProjectiles = (Level+1) / 2; 
    NumberOfProjectiles = BaseNumberOfProjectiles + ExtraProjectiles;
    
    int32 EvenLevelSteps = Level;
    float DamageMultiplier = 1.0f * EvenLevelSteps; 
    RadialDamage = BaseRadialDamage  + (NewPowerUpLevel * 5.f);


    UE_LOG(LogTemp, Log, TEXT("RadialSkill upgraded: Level %d, Projectiles: %d, Damage: %f"), Level, NumberOfProjectiles, RadialDamage);
}