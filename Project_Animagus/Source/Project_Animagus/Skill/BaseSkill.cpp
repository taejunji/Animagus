#include "BaseSkill.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../Character/BaseCharacter.h"  // 소유자 캐릭터 클래스 헤더


UBaseSkill::UBaseSkill()
{
    // 기본값 초기화 (필요에 따라 값을 수정하세요)
    SkillName = "DefaultSkill";
    SkillDescription = "This is a default skill.";
    
    CooldownTime = 5.0f;

    bIsCooldown = false;
    LastUsedTime = -CooldownTime; // 처음에는 사용 가능하도록

    // 첫 사용 플래그 초기화
    bFirstUse = true;
}


void UBaseSkill::ActiveSkill_Implementation()

{
    UE_LOG(LogTemp, Log, TEXT("UBaseSkill::ActiveSkill_Implementation() called on %s"), *GetName());
    
    if (!CanActivateSkill())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShieldSkill: Cannot activate - Owner is null or skill is on cooldown. CurrentCooldown: %f"), GetCooldownPercent());
        return;
    }
    
    // 스킬 실행 로직 (파생 클래스에서 구체적으로 구현)
    UE_LOG(LogTemp, Log, TEXT("BaseSkill::ActiveSkill_Implementation() - Skill %s activated."), *SkillName);

    // 첫 사용이면 플래그 변경
    if (bFirstUse)
    {
        bFirstUse = false;
    }
    
    // 스킬 사용 후 쿨다운 시작
    StartCooldown();


}


void UBaseSkill::DeactiveSkill_Implementation()
{
    // 기본 스킬 비활성화 로직: 필요시 서브 클래스에서 오버라이드하세요.
}

void UBaseSkill::StartCooldown()
{
    if (!Owner || !Owner->GetWorld())
    {
        // 소유자나 월드가 유효하지 않으면 바로 종료
        return;
    }

    bIsCooldown = true;
    LastUsedTime = Owner->GetWorld()->GetTimeSeconds();

    // 타이머를 통해 쿨다운 종료를 예약합니다.
    FTimerDelegate TimerDel;
    TimerDel.BindUFunction(this, FName("EndCooldown"));

    FTimerHandle TimerHandle;
    Owner->GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        TimerDel,
        CooldownTime,
        false
    );
}

void UBaseSkill::EndCooldown()
{
    bIsCooldown = false;
}

bool UBaseSkill::IsOnCooldown() const
{
    if (!Owner || !Owner->GetWorld())
    {
        return false;
    }

    float CurrentTime = Owner->GetWorld()->GetTimeSeconds();
    return (CurrentTime - LastUsedTime) < CooldownTime;
}

float UBaseSkill::GetCooldownPercent() const
{
    if (!bIsCooldown)
    {
        return 1.0f;
    }

    UWorld* World = Owner ? Owner->GetWorld() : nullptr;
    if (!World)
    {
        return 0.0f;
    }
    float CurrentTime = World->GetTimeSeconds();
    float Elapsed = CurrentTime - LastUsedTime;
    // 쿨타임이 지난 경우 1.0, 아니면 비율을 반환
    return FMath::Clamp(Elapsed / CooldownTime, 0.0f, 1.0f);
}

bool UBaseSkill::CanActivateSkill() const
{
    if (!Owner)
    {
        UE_LOG(LogTemp, Warning, TEXT("CanActivateSkill: Owner is NULL for skill %s"), *SkillName);
        return false;
    }
    
    if (!bFirstUse && IsOnCooldown())
    {
        UE_LOG(LogTemp, Warning, TEXT("CanActivateSkill: Skill %s is on cooldown. Cooldown Percent: %f"), *SkillName, GetCooldownPercent());
        return false;
    }
    
    return true;
}