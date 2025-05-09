// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "AICharacter.h"
#include "../AI/MyAIController.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AISense_Damage.h"

#include "NiagaraComponent.h"

#include "../Projectile/ProjectileBase.h"

#include "../Skill/BaseSkill.h"
#include "../Skill/Fireball.h"
#include "../Skill/MagicMissile.h"

#include "Project_Animagus/Skill/Bounce.h"
#include "Project_Animagus/Skill/ChangeSkill.h"
#include "Project_Animagus/Skill/HasteSkill.h"
#include "Project_Animagus/Skill/RadialSkill.h"
#include "Project_Animagus/Skill/ShieldSkill.h"
#include "Project_Animagus/Skill/ShockwaveSkill.h"
#include "Project_Animagus/Skill/SmokeSkill.h"
#include "Project_Animagus/Skill/Stun.h"

#include "../System/MyGameInstance.h"
#include "../Server/Server/protocol.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"

ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    Skills.SetNum(5); 

    PowerUpLevel = 0;

    skill_Sellect = 0;
    
        // 애님 인스턴스 설정
    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBP(TEXT("/Game/WorkFolder/Animation/AnimSystem/ABP_AnimationSystem.ABP_AnimationSystem_C"));
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

    static ConstructorHelpers::FClassFinder<USmokeSkill> SmokeBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/BP_SmokeSkill"));
    if (SmokeBPClassFinder.Succeeded())
    {
        SmokeBPClass = SmokeBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded SmokeBPClassFinder: %s"), *SmokeBPClass -> GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load SmokeBPClassFinder!"));
    }

    static ConstructorHelpers::FClassFinder<UShieldSkill> ShieldBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyShieldSkill"));
    if (ShieldBPClassFinder.Succeeded())
    {
        ShieldBPClass = ShieldBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded ShieldBPClassFinder: %s"), *ShieldBPClass -> GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load ShieldBPClassFinder!"));
    }

    static ConstructorHelpers::FClassFinder<UShockwaveSkill> ShockwaveBPClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyShockwaveSkill"));
    if (ShockwaveBPClassFinder.Succeeded())
    {
        ShockwaveBPClass = ShockwaveBPClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded ShockBPClassFinder: %s"), *ShockwaveBPClass -> GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load ShockBPClassFinder!"));
    }

    static ConstructorHelpers::FClassFinder<UHasteSkill> HasteSkillBpClassFinder(TEXT("/Game/WorkFolder/Bluprints/Skills/MyHasteSkill"));
    if (HasteSkillBpClassFinder.Succeeded())
    {
        HasteBPClass = HasteSkillBpClassFinder.Class;
        UE_LOG(LogTemp, Log, TEXT("BaseCharacter: Successfully loaded HasteBPClassFinder: %s"), *HasteBPClass -> GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseCharacter: Failed to load HasteBPClassFinder!"));
    }
    
    bIsStunned = false;

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

    SpeedBoostComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpeedBoostComponent"));
    SpeedBoostComponent->SetupAttachment(GetMesh());   // 메쉬에 딸려다니도록
    SpeedBoostComponent->bAutoActivate = false;               // 기본 꺼진 상태
    
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
    GetCharacterMovement()->AirControl = 0.7f; // 기본 AirControl은 보통 0.2 ~ 0.3 정도임. 높이면 공중 이동이 민감해짐.

    UE_LOG(LogTemp, Log, TEXT("BaseCharacter::BeginPlay() - Capsule Collision Response for Shockwave: %d"),
    (int)GetCapsuleComponent()->GetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2));
    
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
            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
    //hp -= ActualDamage;

    // 1. Move 패킷에 현재 내 HP 정보를 포함시킨다. 내 캐릭터, AI에 대한 데미지 검사만 실시한다.


    // 2. 내가 데미지를 받으면 서버로 패킷을 전송한다. 서버에서 데미지를 계산해 Broadcast. 다른 클라에서는 그걸로 SetHp
    if (GetPawnType() == PawnType::PLAYER || GetPawnType() == PawnType::AI)
    {
        hp -= ActualDamage;

        Protocol::CS_DAMAGE_PKT DamagePkt;
        DamagePkt.player_id = GetPlayerId();
        DamagePkt.room_id = 0;                // TODO: 로비에서 룸 선택 그거
        DamagePkt.hp = GetHP();
        DamagePkt.isAlive = (hp <= 0.f);

        SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(DamagePkt);
        Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
    }

    // AI에게 데미지를 알림
    if (AAICharacter* AI = Cast<AAICharacter>(this))
    {
        if (AProjectileBase* skillCauser = Cast<AProjectileBase>(DamageCauser)) 
        {
            if (AActor* InstigatorActor = skillCauser->Shooter)  
            {
                AMyAIController* AIC = Cast<AMyAIController>(AI->GetController());
                    
                auto* Target = AIC->GetBlackboardComponent()->GetValueAsObject(AIC->TargetKey.SelectedKeyName); 

                if (nullptr == Target)
                {
                    AIC->GetBlackboardComponent()->SetValueAsBool(AIC->can_set_target_key.SelectedKeyName, true);

                    UAISense_Damage::ReportDamageEvent( 
                        GetWorld(), 
                        this,
                        InstigatorActor, 
                        ActualDamage, 
                        InstigatorActor->GetActorLocation(), 
                        GetActorLocation()  
                    );
                }
            }
        }
    }

    if (hp <= 0)
    {
        // 사망 처리 로직...

        // AI 사망 처리 
        if (AAICharacter* AI = Cast<AAICharacter>(this))
        {
            if (AMyAIController* AIC = Cast<AMyAIController>(AI->GetController()))
            {
                // AI가 죽었으면 Behavior Tree를 멈춤
                if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(AIC->BrainComponent))
                {
                    AIC->SetControlMode(AIControlMode::AIController);
                    AIC->ClearFocus(EAIFocusPriority::Gameplay);  // Focus 해제
                    BehaviorTreeComponent->StopTree();
                }
            }
        }
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
    if (ShieldBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UShieldSkill>(this, ShieldBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[3] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("InitializeSkills: Successfully created Shield skill for slot 3: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: Failed to create Shield skill for slot 3"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: ShieldBPClass is not assigned."));
    }
    
    for (UBaseSkill* Skill : Skills)
    {
        if (Skill)
        {
            Skill->UpgradeSkill(PowerUpLevel);
        }
    } 
}

void ABaseCharacter::TestSkill_Change()
{
    // 슬롯 0: UBounce 스킬 생성
    if (HasteBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UHasteSkill>(this, HasteBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[0] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("TestSkill_Change: Successfully created Bounce skill for slot 0: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("TestSkill_Change: Failed to create Bounce skill for slot 0"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TestSkill_Change: BounceBPClass is not assigned."));
    }

    // 슬롯 1: UStun 스킬 생성
    if (StunBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UStun>(this, StunBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[1] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("TestSkill_Change: Successfully created Stun skill for slot 1: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("TestSkill_Change: Failed to create Stun skill for slot 1"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TestSkill_Change: StunBPClass is not assigned."));
    }

    // 슬롯 2: UChangeSkill 스킬 생성
    if (ChangeBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UChangeSkill>(this, ChangeBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[2] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("TestSkill_Change: Successfully created Change skill for slot 2: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("TestSkill_Change: Failed to create Change skill for slot 2"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TestSkill_Change: ChangeBPClass is not assigned."));
    }

    // 슬롯 3: USmokeSkill 스킬 생성
    if (SmokeBPClass)
    {
        UBaseSkill* NewSkill = NewObject<USmokeSkill>(this, SmokeBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[3] = NewSkill;
            UE_LOG(LogTemp, Log, TEXT("TestSkill_Change: Successfully created Smoke skill for slot 3: %s"), *NewSkill->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("TestSkill_Change: Failed to create Smoke skill for slot 3"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TestSkill_Change: SmokeBPClass is not assigned."));
    }

    for (UBaseSkill* Skill : Skills)
    {
        if (Skill)
        {
            Skill->UpgradeSkill(PowerUpLevel);
        }
    }
}

void ABaseCharacter::AIchar_SkillSet()
{
    // PowerUpLevel 적용 전 리셋
    Skills.Empty();
    Skills.SetNum(4);
    // 1) 사용할 수 있는 스킬 BP 클래스를 TArray로 모은다
    TArray<TSubclassOf<UBaseSkill>> AllSkillClasses = {
        //FireballBPClass,
        MagicMissileBPClass,
        StunBPClass,
        RadialBPClass,
        ChangeBPClass,
        SmokeBPClass,
        ShieldBPClass,
        //ShockwaveBPClass,
        HasteBPClass,
    };

    // 2) 올바르게 설정된(널이 아닌) 클래스만 필터링
    AllSkillClasses.RemoveAll([](TSubclassOf<UBaseSkill> Cls) { return Cls == nullptr; });

    // 3) 랜덤 셔플
    const int32 NumToPick = FMath::Min(4, AllSkillClasses.Num());
    for (int32 i = AllSkillClasses.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        AllSkillClasses.Swap(i, j);
    }

    // 4) 앞에서부터 4개를 순서대로 슬롯에 NewObject로 생성
    for (int32 Slot = 0; Slot < NumToPick; ++Slot)
    {
        TSubclassOf<UBaseSkill> SkillClass = AllSkillClasses[Slot];
        if (SkillClass)
        {
            // UObject::NewObject<>() 템플릿으로 생성
            UBaseSkill* NewSkill = NewObject<UBaseSkill>(this, SkillClass);
            if (NewSkill)
            {
                NewSkill->Owner = this;
                Skills[Slot] = NewSkill;
                UE_LOG(LogTemp, Log, TEXT("InitializeSkills: Slot %d -> %s"), Slot, *NewSkill->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("InitializeSkills: Failed to NewObject for slot %d"), Slot);
            }
        }
    }
    
    if (ShockwaveBPClass)
    {
        UBaseSkill* NewSkill = NewObject<UShockwaveSkill>(this, ShockwaveBPClass);
        if (NewSkill)
        {
            NewSkill->Owner = this;
            Skills[1] = NewSkill;
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

    // 5) PowerUpLevel에 따른 강화 적용
    for (UBaseSkill* Skill : Skills)
    {
        if (Skill)
        {
            Skill->UpgradeSkill(PowerUpLevel);
        }
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

bool ABaseCharacter::UseSkillByName(const FString& DesiredSkillName)
{
    for (UBaseSkill* Skill : Skills)
    {
        if (!Skill) continue;

        if (Skill->SkillName == DesiredSkillName && Skill->CanActivateSkill())
        {
            // UE_LOG(LogTemp, Warning, TEXT("Used skill: %s"), *DesiredSkillName);

            FRotator Rotation;

            if (AMyAIController* AIController = Cast<AMyAIController>(GetController()))
            {
                ABaseCharacter* TargetCharacter = nullptr;
                UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
                if (BBComp && AIController->TargetKey.SelectedKeyName.IsValid())
                {
                    TargetCharacter = Cast<ABaseCharacter>(BBComp->GetValueAsObject(AIController->TargetKey.SelectedKeyName));
                }
                if (TargetCharacter)
                {
                    FVector DirectionToTarget = (TargetCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                    Rotation = DirectionToTarget.Rotation();
                }
                else
                {
                    // 타겟이 없다면 AI Panw이 바라보는 방향으로 발사
                    Rotation = GetActorRotation();
                }

                Protocol::CS_AI_USING_SKILL_PKT SkillPkt;
                SkillPkt.ai_id = GetPlayerId();
                SkillPkt.s_type = Skill->SkillType;
                //SkillPkt.x = Location.X; SkillPkt.y = Location.Y; SkillPkt.z = Location.Z;  // 필수인가?
                SkillPkt.pitch = Rotation.Pitch; SkillPkt.yaw = Rotation.Yaw; SkillPkt.roll = Rotation.Roll;

                SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(SkillPkt);
                Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
            }

            Skill->SetSkillRotation(Rotation.Pitch, Rotation.Yaw, Rotation.Roll);
            Skill->ActiveSkill();


            return true; // 하나만 사용하고 끝내려면 여기서 리턴
        }
    }

    return false;
    /*if (Skills[0]->SkillName != TEXT("Haste"))
        UE_LOG(LogTemp, Warning, TEXT("Skill not found: %s"), *DesiredSkillName);*/
}

void ABaseCharacter::IncreasePowerUpLevel()
{
    if (PowerUpLevel > 14)
    {
        return;
    }
    
    PowerUpLevel++;
    UE_LOG(LogTemp, Log, TEXT("%s PowerUpLevel increased to %d"), *GetName(), PowerUpLevel);

    // 보유한 모든 스킬에 대해 UpgradeSkill() 호출
    for (UBaseSkill* Skill : Skills)
    {
        if (Skill)
        {
            Skill->UpgradeSkill(PowerUpLevel);
        }
    }

    hp += 10.f;
    max_hp += 10.f;
    
    UpdateAuraColorBasedOnPowerUpLevel(); 
    // HUD 업데이트 등 추가 작업 가능 (예: 플레이어 머리 위에 현재 강화 단계를 표시)
}

void ABaseCharacter::UpdateAuraColorBasedOnPowerUpLevel()
{
    
    if (!AuraMaterialInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateAuraColorBasedOnPowerUpLevel: Failed to get dynamic material instance."));
        return;
    }

    FLinearColor NewColor;
    // PowerUpLevel에 따라 색상 결정 (예시: 1~2: Red, 3~4: Orange, 5~6: Yellow, 7~8: Green, 9~10: Blue, 11~12: Indigo, 13~14: Purple)
    if (PowerUpLevel == 1 || PowerUpLevel == 7)
    {
        NewColor = FLinearColor::Red;
    }
    else if (PowerUpLevel == 2 || PowerUpLevel == 8)
    {
        NewColor = FLinearColor(1.0f, 0.25f, 0.0f, 0.5f); // Orange
    }
    else if (PowerUpLevel == 3 || PowerUpLevel == 9)
    {
        NewColor = FLinearColor::Yellow;
    }
    else if (PowerUpLevel == 4 || PowerUpLevel == 10)
    {
        NewColor = FLinearColor::Green;
       
    }
    else if (PowerUpLevel == 5 || PowerUpLevel == 11)
    {
        NewColor = FLinearColor::Blue;
    }
    else if (PowerUpLevel == 6 || PowerUpLevel == 12)
    {
        NewColor = FLinearColor(0.29f, 0.0f, 0.51f); // Indigo (근사치)
    }
    else
    {
        NewColor = FLinearColor(1.5f, 3.0f, 8.5f);
    }

    if (PowerUpLevel > 6)
    {
        AuraMaterialInstance->SetScalarParameterValue(FName("Power"), 18.f);
    }

    // "auracolor" 파라미터 업데이트
    AuraMaterialInstance->SetVectorParameterValue(FName("BaseColor"), NewColor);

    UE_LOG(LogTemp, Log, TEXT("UpdateAuraColorBasedOnPowerUpLevel: Updated auracolor to %s for PowerUpLevel %d"), *NewColor.ToString(), PowerUpLevel);
}

void ABaseCharacter::PickHealingItem()
{
    float Heal  = max_hp / 3;
    hp += Heal;
    if (hp > max_hp) {
        hp = max_hp;
    }
}

void ABaseCharacter::PickHealingPlusItem()
{
    float Heal  = max_hp / 2;
    hp += Heal;
    if (hp > max_hp) {
        hp = max_hp;
    }
}
