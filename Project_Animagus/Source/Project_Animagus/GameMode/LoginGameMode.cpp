// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "../System//MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerController/LoginPlayerController.h"
#include "../Server/Server/protocol.h"

ALoginGameMode::ALoginGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

}

void ALoginGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    //if (auto* GI = Cast<UMyGameInstance>(GetGameInstance()))
    //{
    //    GI->ResetLoginBGM();
    //}
}

void ALoginGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Cast<UMyGameInstance>(GWorld->GetGameInstance())->HandleRecvPackets();
}

void ALoginGameMode::HandleLoginSuccess()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (nullptr == PC) return;
    if (ALoginPlayerController* LoginPC = Cast<ALoginPlayerController>(PC))
    {
        LoginPC->HandleLoginSuccess();

        // 약간 좀 느낌있게 1초 후 OpenLevel
        FTimerHandle UnusedHandle;
        GetWorldTimerManager().SetTimer(
            UnusedHandle,
            [this]()
            {
                UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/L_MainMenu"));
            },
            1.0f,
            false  // 한번만
        );

    }
}

void ALoginGameMode::HandleLoginFail(Protocol::SC_LOGIN_FAIL_PKT& pkt)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (nullptr == PC) return;
    if (ALoginPlayerController* LoginPC = Cast<ALoginPlayerController>(PC))
    {
        LoginPC->HandleLoginFail(pkt);
    }
}

void ALoginGameMode::HandleSignUpSuccess()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (nullptr == PC) return;
    if (ALoginPlayerController* LoginPC = Cast<ALoginPlayerController>(PC))
    {
        LoginPC->HandleSignUpSuccess();
    }
}

void ALoginGameMode::HandleSignUpFail()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (nullptr == PC) return;
    if (ALoginPlayerController* LoginPC = Cast<ALoginPlayerController>(PC))
    {
        LoginPC->HandleSignUpFail();
    }
}
