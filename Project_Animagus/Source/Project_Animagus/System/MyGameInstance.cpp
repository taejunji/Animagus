// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../Character/AICharacter.h"
#include "Components/AudioComponent.h"

UMyGameInstance::UMyGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

    // 메쉬 애셋 저장 -> ConstructorHelpers는 생성자에서만 호출해야한다.
    // static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Tiger(TEXT("/Game/WorkFolder/Assets/Tiger/Character128_RiggedTiger.Character128_RiggedTiger"));
    // if (SM_Tiger.Succeeded()) CharacterMeshes.Add(TEXT("SM_Tiger"), SM_Tiger.Object);

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Monkey(TEXT("/Game/WorkFolder/Assets/Monkey/Character134_RiggedMonkey.Character134_RiggedMonkey"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Koala(TEXT("/Game/WorkFolder/Assets/Koala/Character139_RiggedKoala.Character139_RiggedKoala"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Sheep(TEXT("/Game/WorkFolder/Assets/Sheep/Character144_RiggedSheep.Character144_RiggedSheep"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Fox(TEXT("/Game/WorkFolder/Assets/Fox/Character131_RiggedFox.Character131_RiggedFox"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Sloth(TEXT("/Game/WorkFolder/Assets/Sloth/Character149_RiggedSloth.Character149_RiggedSloth")); 
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Elephant(TEXT("/Game/WorkFolder/Assets/Elephant/Character136_RiggedElephant.Character136_RiggedElephant"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Raccoon(TEXT("/Game/WorkFolder/Assets/Raccoon/Character133_RiggedRaccoon.Character133_RiggedRaccoon"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Deer(TEXT("/Game/WorkFolder/Assets/Deer/Character146_RiggedDeer.Character146_RiggedDeer"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Cow(TEXT("/Game/WorkFolder/Assets/Cow/Character132_RiggedCow.Character132_RiggedCow")); 
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Unicorn(TEXT("/Game/WorkFolder/Assets/Unicorn/Character143_RiggedUnicorn.Character143_RiggedUnicorn")); 
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Zebra(TEXT("/Game/WorkFolder/Assets/Zebra/Character142_RiggedZebra.Character142_RiggedZebra")); 
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_Donkey(TEXT("/Game/WorkFolder/Assets/Donkey/Character148_RiggedDonkey.Character148_RiggedDonkey")); 

    if (SM_Monkey.Succeeded()) CharacterMeshes.Add(TEXT("SM_Monkey"), SM_Monkey.Object);
    if (SM_Koala.Succeeded()) CharacterMeshes.Add(TEXT("SM_Koala"), SM_Koala.Object);
    if (SM_Sheep.Succeeded()) CharacterMeshes.Add(TEXT("SM_Sheep"), SM_Sheep.Object);
    if (SM_Fox.Succeeded()) CharacterMeshes.Add(TEXT("SM_Fox"), SM_Fox.Object);

    if (SM_Sloth.Succeeded()) CharacterMeshes.Add(TEXT("SM_Sloth"), SM_Sloth.Object); 
    if (SM_Elephant.Succeeded()) CharacterMeshes.Add(TEXT("SM_Elephant"), SM_Elephant.Object); 
    if (SM_Raccoon.Succeeded()) CharacterMeshes.Add(TEXT("SM_Raccoon"), SM_Raccoon.Object); 
    if (SM_Deer.Succeeded()) CharacterMeshes.Add(TEXT("SM_Deer"), SM_Deer.Object);

    if (SM_Cow.Succeeded()) CharacterMeshes.Add(TEXT("SM_Cow"), SM_Cow.Object);
    if (SM_Unicorn.Succeeded()) CharacterMeshes.Add(TEXT("SM_Unicorn"), SM_Unicorn.Object);
    if (SM_Zebra.Succeeded()) CharacterMeshes.Add(TEXT("SM_Zebra"), SM_Zebra.Object); 
    if (SM_Donkey.Succeeded()) CharacterMeshes.Add(TEXT("SM_Donkey"), SM_Donkey.Object);

}

void UMyGameInstance::Init()
{
    Super::Init();

    // 데이터 불러왔을 때 mesh 타입 설정하기 => ( 임시로 양 디폴트 )
    player_data.stored_mesh = CharacterMesh::Sheep; 

    InitGameInstance();
}

void UMyGameInstance::Shutdown()
{
    Super::Shutdown();
}

void UMyGameInstance::InitGameInstance()
{
    // 배틀 처음 시작 시 -> 라운드 0, 레벨 Battle로 설정
    // 0: 아침
    // 1: 저녁
    // 2: 밤
    round_count = 0;
    // current_level = LevelType::Battle;
}

void UMyGameInstance::SwitchLevel(LevelType level)
{
    switch (level)
    {
    case LevelType::Login:
        UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/Login"));
        break;

    case LevelType::Lobby:
        UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/Lobby"));
        break;

    case LevelType::Battle:
        UGameplayStatics::OpenLevel(GetWorld(), FName("/Game/WorkFolder/Levels/L_Map"));
        break;
    }
}

void UMyGameInstance::PlayMenuBGM()
{
    if (!MenuBGM || MenuBGMComponent)
        return;

    // 월드가 없으면 못 만듦
    if (UWorld* W = GetWorld())
    {
        MenuBGMComponent = UGameplayStatics::SpawnSound2D(W, MenuBGM);
        if (MenuBGMComponent)
        {
            MenuBGMComponent->bIsUISound = true;  // UI 사운드로 분류
            MenuBGMComponent->Play();
        }
    }
}

void UMyGameInstance::StopMenuBGM()
{
    if (MenuBGMComponent)
    {
        MenuBGMComponent->Stop();
        MenuBGMComponent = nullptr;
    }
}

//void UMyGameInstance::AddAICharacter(AAICharacter* AICharacter)
//{
//    if (AICharacter)
//    {
//        AIPlayers.Add(AICharacter);
//    }
//}

void UMyGameInstance::PrintGameInstanceData()
{
    FString DebugMessage = FString::Printf(TEXT("게임 인스턴스 - 배틀 라운드 수: %d"), round_count);
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, DebugMessage);
}
