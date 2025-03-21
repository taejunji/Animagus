#include "RadialSkill.h"
#include "../Projectile/ProjectileBase.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"

URadialSkill::URadialSkill()
{
    SkillName = "RadialSkill";           // 스킬 이름 설정
    CooldownTime = 8.0f;                 // 쿨타임 8초
    NumberOfProjectiles = 7;             // 7개의 투사체 발사
    SpreadAngle = 90.0f;                 // 전체 90도 부채꼴
    RadialDamage = 40.0f;                // 투사체 데미지
    RadialSpeed = 1600.f;                // 투사체 속도

    SpawnRadius = 80.f;                  // 캐릭터 기준으로 80cm 떨어진 위치에서 스폰
    VerticalOffset = 20.f;               // 캐릭터 기준으로 위쪽 20cm

    ProjectileBPClass = nullptr;         // 에디터에서 할당 필요
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

    // 진행 방향: 카메라가 바라보는 중심 방향을 사용
    FVector CameraLocation;
    FRotator CameraRotation;
    if (Owner->GetController())
    {
        Owner->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
        UE_LOG(LogTemp, Log, TEXT("URadialSkill: CameraLocation = %s, CameraRotation = %s"), *CameraLocation.ToString(), *CameraRotation.ToString());
    }
    else
    {
        CameraRotation = Owner->GetActorRotation();
        UE_LOG(LogTemp, Log, TEXT("URadialSkill: Controller 없음, CameraRotation = %s"), *CameraRotation.ToString());
    }
    
    // 각 투사체의 발사 각도 계산
    float BaseYaw = CameraRotation.Yaw;
    float AngleStep = (NumberOfProjectiles > 1) ? SpreadAngle / (NumberOfProjectiles - 1) : 0.f;
    float StartYaw = BaseYaw - (SpreadAngle * 0.5f);
    UE_LOG(LogTemp, Log, TEXT("URadialSkill: BaseYaw = %f, AngleStep = %f, StartYaw = %f"), BaseYaw, AngleStep, StartYaw);
    
    // 캐릭터의 위치를 기준으로 투사체의 스폰 위치 계산
    FVector CharacterLocation = Owner->GetActorLocation();
    
    // 한 번에 모든 투사체 생성
    for (int32 i = 0; i < NumberOfProjectiles; i++)
    {
        // 현재 투사체의 Yaw 계산
        float CurrentYaw = StartYaw + i * AngleStep;
        // 수평 스폰 위치: 캐릭터 위치에서 현재 각도에 따른 단위 벡터에 SpawnRadius를 곱함
        FRotator HorizontalRot(0.f, CurrentYaw, 0.f);
        FVector SpawnLocation = CharacterLocation + HorizontalRot.Vector() * SpawnRadius + FVector(0.f, 0.f, VerticalOffset);
        FRotator SpawnRotation = FRotator(CameraRotation.Pitch, CurrentYaw, CameraRotation.Roll);
        
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
