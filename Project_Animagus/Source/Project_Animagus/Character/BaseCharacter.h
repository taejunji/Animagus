// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

/*
*   Base Character는 3종류의 캐릭터에 대한 근본이 되는 클래스
        Player Characetr  : 플레이어 기준   <- 키보드, 마우스 컨트롤러  
        Network Character : 온라인 플레이어 <- 이동, 회전 등 패킷값을 받아 렌더링  
        AI Characeter     : 플레이어 중 한명이 AI의 행동을 담당하고 나머지는 그런 데이터를 받고 렌더링

    공통되는 함수나 애니메이션 애셋을 공유할 것이다.

    Animation Montage (애니메이션 몽타주) : 특정한 애니메이션을 재생 - 죽음, 피격, 어택 등 
 
*/

enum class MontageType { DefaultAttack, Hit };

UCLASS()
class PROJECT_ANIMAGUS_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float default_walk_speed; // 기본 걷기 속도
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float default_run_speed; // 최대 달리기 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float current_speed; // 현재 속도

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float speed_change_rate; // 속도 변화 비율 ( 30.f면 1초에 30씩 변화 )

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float hp;   // HP 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float max_hp;   // MaxHP 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    bool is_dead; // 죽었는지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    bool is_stun; // 스턴 상태인지

protected:
    UPROPERTY(EditAnywhere, Category = "AnimationMontage")
    TObjectPtr<class UAnimMontage> attack_montage;
    UPROPERTY(EditAnywhere, Category = "AnimationMontage")
    TObjectPtr<class UAnimMontage> hit_montage;

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
    virtual void PlayAnimMontageByType(MontageType montage_type);

    void SetWalkSpeed(float fValue);
    void SetHP(float fValue) { hp = fValue; }
    void SetIsHardHit(bool bValue) { is_stun = bValue; }

    float GetHP() const { return hp; }
    bool GetIsDead() const { return is_dead; }
    bool GetIsHardHit() const { return is_stun; }

public:
    // 스킬 관련 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
    TSubclassOf<class AProjectileSkill> projectile_class; // BP Projectile 클래스 정보

     virtual void FireProjectile();

};

// Called to bind functionality to input
// virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
// 
//void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}