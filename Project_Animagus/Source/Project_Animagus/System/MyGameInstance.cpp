// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"


UMyGameInstance::UMyGameInstance()
{
    InitGameInstance();
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("게임 인스턴스 초기화"));
    }
}

void UMyGameInstance::InitGameInstance()
{
    round_count = 0;
    current_level = LevelType::Battle;
}

void UMyGameInstance::SwitchLevel(LevelType level)
{
    switch (level)
    {
    case LevelType::Login:
        UGameplayStatics::OpenLevel(GetWorld(), FName("Login"));
        break;

    case LevelType::Lobby:
        UGameplayStatics::OpenLevel(GetWorld(), FName("Lobby"));
        break;

    case LevelType::Battle:
        UGameplayStatics::OpenLevel(GetWorld(), FName("InGameBattle"));
        break;
    }
}

void UMyGameInstance::PrintGameInstanceData()
{
    FString DebugMessage = FString::Printf(TEXT("게임 인스턴스 - 배틀 라운드 수: %d"), round_count);
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, DebugMessage);
}
