// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

#include "../Skill/BaseSkill.h"
#include "../Skill/Fireball.h"
#include "../Skill/MagicMissile.h"
#include "Project_Animagus/Skill/Bounce.h"
#include "Project_Animagus/Skill/ChangeSkill.h"
#include "Project_Animagus/Skill/RadialSkill.h"
#include "Project_Animagus/Skill/Stun.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    Skills.SetNum(5); 

    // 애님 인스턴스 설정
    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/Game/WorkFolder/Animation/AnimSystem/ABP_TEST2.ABP_TEST2_C"));
    if (AnimBP.Succeeded())
    {
        GetMesh()->SetAnimInstanceClass(AnimBP.Class);
    }




    // ConstructorHelpers를 사용하여 UFireball 블루프린트 클래스 로드
    static ConstructorHelpers::FClassFinder<UFireball> FireballBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyFireball"));
    if (FireballBPClassFinder.Succeeded())
    {
        FireballBPClass = FireballBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded FireballBPClass: %s"), *FireballBPClass->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load FireballBPClass!"));
    }
    
    // ConstructorHelpers를 사용하여 MagicMissile 블루프린트 클래스 로드
    static ConstructorHelpers::FClassFinder<UMagicMissile> MagicMissileBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyMagicMissile"));
    if (MagicMissileBPClassFinder.Succeeded())
    {
        MagicMissileBPClass = MagicMissileBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded MagicMissileBPClassFinder: %s"), *MagicMissileBPClass->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load MagicMissileBPClassFinder!"));
    }

    static ConstructorHelpers::FClassFinder<UBounce> BounceBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyBounce"));
    if (BounceBPClassFinder.Succeeded())
    {
        BounceBPClass = BounceBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded BounceBPClassFinder: %s"), *BounceBPClass -> GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load BounceBPClassFinder!"));
    } 

    static ConstructorHelpers::FClassFinder<UStun> StunBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyStun"));
    if (StunBPClassFinder.Succeeded())
    {
        StunBPClass = StunBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded StunBPClassFinder: %s"), *StunBPClass -> GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load StunBPClassFinder!"));
    } 

static ConstructorHelpers::FClassFinder<URadialSkill> RadialBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyRadialSkill"));
    if (RadialBPClassFinder.Succeeded())
    {
        RadialBPClass = RadialBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded RadialBPClassFinder: %s"), *RadialBPClass -> GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load RadialBPClassFinder!"));
    }

    static ConstructorHelpers::FClassFinder<UChangeSkill> ChangeBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyChangeSkill"));
    if (ChangeBPClassFinder.Succeeded())
    {
        ChangeBPClass = ChangeBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded ChangeBPClassFinder: %s"), *ChangeBPClass -> GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load ChangeBPClassFinder!"));
    }

    
    bIsStunned = false;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

    // 점프 속도를 500으로 설정
    // 기본 걷기 속도 300으로 설정 - 1초에 3m
    GetCharacterMovement()->JumpZVelocity = 500.0f;

    default_walk_speed = 250.f;
    default_run_speed = 500.f;
    speed_change_rate = 5.f; // 1초에 5.f정도의 속도 변화를 꿈 꿨는데 뭔가 이상하다 
    current_speed = default_walk_speed;

    max_hp = 100.f;
    hp = max_hp;
    is_dead = false;
    bIsStunned = false;

    SetWalkSpeed(default_walk_speed);

    // 공중 제어 능력 높임. 기본값이 낮으면 공중에서 이동키가 약하게 반응함.
    GetCharacterMovement()->AirControl = 0.8f; // 기본 AirControl은 보통 0.2 ~ 0.3 정도임. 높이면 공중 이동이 민감해짐.

    
    InitializeSkills();
}

void ABaseCharacter::PlayAnimMontageByType(MontageType montage_type)
{
    UAnimMontage* SelectMontage = nullptr;

    // 특정 애니메이션 재생 -> Locomotion(이동관련) 애니메이션 상태머신과 "상하체 블랜딩" 됨
    switch (montage_type)
    {
    case MontageType::DefaultAttack:
        SelectMontage = attack_montage;
        break;

    case MontageType::Hit:
        SelectMontage = hit_montage;
        break;
    }

    if (SelectMontage)
    {
        PlayAnimMontage(SelectMontage);
    }
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 임시 캐릭터 죽음 테스트
    if (hp <= 0.f) {        
        if (is_dead == false) is_dead = true;
        if (GetCharacterMovement()->IsFalling() == false) {
            // 일시적으로 이동을 멈추고 싶다면? → DisableMovement()
            // 이동을 완전히 비활성화하고 싶다면 ? → SetMovementMode(MOVE_None)
            GetCharacterMovement()->DisableMovement();
        }
        return;
    }
    else {
        is_dead = false;
    } 

}

void ABaseCharacter::SetWalkSpeed(float fValue)
{
    GetCharacterMovement()->MaxWalkSpeed = fValue;
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    PlayAnimMontageByType(MontageType::Hit);
    hp -= ActualDamage;

    // 나중에 패킷관리 할 예시 코드 ㅇㅇ

    // FDamagePacket DamagePacket;
    // DamagePacket.CharacterID = this->GetUniqueID(); // 예시로 캐릭터의 고유 ID 사용
    // DamagePacket.DamageAmount = ActualDamage;
    // DamagePacket.DamageCauserID = DamageCauser ? DamageCauser->GetUniqueID() : 0;
    // DamagePacket.Timestamp = GetWorld()->TimeSeconds;
    //
    // // 네트워크 매니저를 통해 서버로 데미지 패킷 전송
    // // MyNetworkManager는 여러분이 구현한 네트워크 시스템 클래스여야 합니다.
    // MyNetworkManager::Get()->SendDamagePacket(DamagePacket);
    
    if (hp <= 0)
    {
        // 사망 처리 로직...
    }
    return ActualDamage;
    //return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
}


void ABaseCharacter::EquipSkill(int32 SlotIndex, UBaseSkill* NewSkill)
{
    if (Skills.IsValidIndex(SlotIndex))
    {
        Skills[SlotIndex] = NewSkill;
    }
}

void ABaseCharacter::InitializeSkills()
{
 // 여기는 스킬 고르는 거 넣기 전에 테스트용임 ㅇㅇ
    
    // 0번슬롯 ㅇㅇ
    if (FireballBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UFireball>(this, FireballBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[0] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("InitializeSkills: Successfully created MagicMissile skill for slot 0: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: Failed to create MagicMissile skill for slot 0"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: FireballBPClass is not assigned."));
    }
    
    // 슬롯 1에 UMagicMissile 스킬 생성 ㅇㅇ
    if (MagicMissileBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UMagicMissile>(this, MagicMissileBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[1] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("InitializeSkills: Successfully created MagicMissile skill for slot 1: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: Failed to create MagicMissile skill for slot 1"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: MagicMissileBPClass is not assigned."));
    }
    
    // 2번슬롯 
    if (RadialBPClass)
    {
        UBaseSkill* NewSkill = NewObject<URadialSkill>(this, RadialBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[2] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("InitializeSkills: Successfully created Radial skill for slot 2: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: Failed to create Radial skill for slot 2"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: RadialBPClass is not assigned."));
    }

    // 3번슬롯 
    if (StunBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UStun>(this, StunBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[3] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("InitializeSkills: Successfully created Stun skill for slot 3: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: Failed to create Stun skill for slot 3"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: StunBPClass is not assigned."));
    }

    // 4번 슬롯 테스트용임
    if (ChangeBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UChangeSkill>(this, ChangeBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[4] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("InitializeSkills: Successfully created Change skill for slot 4: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: Failed to create Change skill for slot 4"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: ChangeBPClass is not assigned."));
    } 
    
}

void ABaseCharacter::ApplyStun(float Duration)
{
    // 이미 스턴 중이면 무시
    if (bIsStunned)
    {
        return;
    }

    bIsStunned = true;
    UE_LOG(LogTemp, Log, TEXT("%s is stunned for %f seconds"), *GetName(), Duration);

    // 플레이어 입력 차단 (플레이어 컨트롤러가 있다면)
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }

    // 타이머 설정: Duration 이후에 RemoveStun() 호출
    FTimerHandle StunTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &ABaseCharacter::RemoveStun, Duration, false);
}

void ABaseCharacter::RemoveStun()
{
    bIsStunned = false;
    UE_LOG(LogTemp, Log, TEXT("%s is no longer stunned"), *GetName());

    // 입력 복구
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
    }
}


