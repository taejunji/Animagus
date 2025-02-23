// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "../Skill/BaseSkill.h"
#include "../Skill/Fireball.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
    
    // 애님 인스턴스 설정
    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/Game/WorkFolder/Animation/AnimSystem/ABP_TEST2.ABP_TEST2_C"));
    if (AnimBP.Succeeded())
    {
        GetMesh()->SetAnimInstanceClass(AnimBP.Class);
    }

    Skills.SetNum(4);

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

    hp = 100.f;
    is_dead = false;
    is_stun = false;

    SetWalkSpeed(default_walk_speed);

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


    }// 제거 
}

void ABaseCharacter::SetWalkSpeed(float fValue)
{
    GetCharacterMovement()->MaxWalkSpeed = fValue;
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    hp -= ActualDamage;
    
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

<<<<<<< Updated upstream
void ABaseCharacter::EquipSkill(int32 SlotIndex, UBaseSkill* NewSkill)
{
    if (Skills.IsValidIndex(SlotIndex))
    {
        Skills[SlotIndex] = NewSkill;
    }
}

void ABaseCharacter::InitializeSkills()
{
    for (int32 i = 0; i < Skills.Num(); i++)
    {
        // UFireballSkill은 UBaseSkill을 상속받은 파이어볼 스킬 클래스입니다.
        UBaseSkill* NewSkill = NewObject<UFireball>(this, UFireball::StaticClass());
        if (NewSkill)
        {
            // 스킬의 소유자(Owner) 설정
            NewSkill->Owner = this;
            Skills[i] = NewSkill;
        }
    }
    
}

=======
>>>>>>> Stashed changes


