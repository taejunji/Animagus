// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "../AI/MyAIController.h"

void AAICharacter::ApplyStun(float Duration)
{
    // 이미 스턴 중이면 무시
    if (bIsStunned)
    {
        return;
    }

    bIsStunned = true;
    UE_LOG(LogTemp, Log, TEXT("%s is stunned for %f seconds"), *GetName(), Duration);

    // 플레이어 입력 차단 (플레이어 컨트롤러가 있다면)
    if (AAIController* PC = Cast<AAIController>(GetController()))
    {
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }

    // 타이머 설정: Duration 이후에 RemoveStun() 호출
    FTimerHandle StunTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(StunTimerHandle, this, &AAICharacter::RemoveStun, Duration, false);
}

void AAICharacter::RemoveStun()
{
    bIsStunned = false;
    UE_LOG(LogTemp, Log, TEXT("%s is no longer stunned"), *GetName());

    // 입력 복구
    if (AAIController* PC = Cast<AAIController>(GetController()))
    {
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
    }
}
