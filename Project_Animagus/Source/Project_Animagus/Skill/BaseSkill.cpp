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
}

void UBaseSkill::ActiveSkill_Implementation()
{
    // 기본 스킬 활성화 로직: 서브 클래스에서 오버라이드 하세요.
    if (!bIsCooldown)
    {
        // 예시: 스킬 사용 후 쿨다운 시작
        StartCooldown();
    }
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
