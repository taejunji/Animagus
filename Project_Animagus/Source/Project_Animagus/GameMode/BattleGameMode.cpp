// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "../System//MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


ABattleGameMode::ABattleGameMode()
{
    PrimaryActorTick.bCanEverTick = false;

    // BluePirnt Class인 BP_Player, BP_PlayerController의 정보를 생성자에서 읽어서 게임모드에 설정한다.
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawn(TEXT("/Game/WorkFolder/Bluprints/BP_Player.BP_Player_C"));
    if (PlayerPawn.Succeeded())
    {
        DefaultPawnClass = PlayerPawn.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("디폴트 폰 로드 실패")); 

    static ConstructorHelpers::FClassFinder<APlayerController> PController(TEXT("/Game/WorkFolder/Controller/BP_TEST_PC_BattleGame.BP_TEST_PC_BattleGame_C"));
    if (PController.Succeeded())
    {
        PlayerControllerClass = PController.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("플레이어 컨트롤러 로드 실패"));

}

void ABattleGameMode::StartPlay()
{
    Super::StartPlay();

    InitBattleMode();
}

void ABattleGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
 
}

void ABattleGameMode::InitBattleMode()
{
    elasped_time = 0.0f;
    GetWorld()->GetTimerManager().ClearTimer(battle_timer_handle); // 타이머가 중지됨

    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));  
    if (MyGameInstance)
    {
        // 1초마다 경과시간 호출 함수 타이머 설정
        GetWorld()->GetTimerManager().SetTimer(battle_timer_handle, this, &ABattleGameMode::PrintElapsedtime, 1.0f, true); 
    }
}

void ABattleGameMode::PrintElapsedtime()
{
    // 경과 시간을 출력
    elasped_time += 1.0f; 
     
    FString DebugMessage = FString::Printf(TEXT("라운드 경과 시간: %f초"), elasped_time);
    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, DebugMessage); 

    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (MyGameInstance)
    {
        // 5초마다 라운드 증가 [ 그냥 테스트 코드 ]
        if ((int)elasped_time % 5 == 0)
        {
            MyGameInstance->round_count++;
        }
        MyGameInstance->PrintGameInstanceData(); 
    }
}
