// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"
#include "../System//MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Character/BaseCharacter.h"
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

    static ConstructorHelpers::FClassFinder<APlayerController> PController(TEXT("/Game/WorkFolder/Controller/BP_Battle_PlayerController.BP_Battle_PlayerController_C"));
    if (PController.Succeeded())
    {
        PlayerControllerClass = PController.Class;
    }
    else UE_LOG(LogTemp, Warning, TEXT("플레이어 컨트롤러 로드 실패"));

    // SpawnLocations 기본값 설정 (에디터에서 재조정 가능)
    SpawnLocations.Add(FVector(0.f, 0.f, 2000.f));
    SpawnLocations.Add(FVector(500.f, 0.f, 2000.f));
    SpawnLocations.Add(FVector(0.f, 500.f, 2000.f));
    SpawnLocations.Add(FVector(500.f, 500.f, 2000.f));

    SpawnRotations.Add(FRotator(0.f, 0.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 90.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 180.f, 0.f));
    SpawnRotations.Add(FRotator(0.f, 270.f, 0.f));
    
    PossessIndex = 0; // 기본적으로 0번 플레이어를 소유하도록 설정
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

    // 먼저 자동으로 생성된 Pawn이 있다면 제거함
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: World가 null임."));
        return;
    }
    
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    if (PC)
    {
        APawn* AutoPawn = PC->GetPawn();
        if (AutoPawn)
        {
            UE_LOG(LogTemp, Log, TEXT("BattleGameMode: 자동 생성된 Pawn %s 제거함."), *AutoPawn->GetName());
            AutoPawn->Destroy();
        }
    }
    
    // 플레이어 캐릭터들을 SpawnLocations 배열에 따라 스폰함
    SpawnedPlayers.Empty();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: World가 null임."));
        return;
    }

    // SpawnLocations 배열에 최소 4개가 있어야 함.
    if (SpawnLocations.Num() < 4)
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: SpawnLocations 수가 충분하지 않음."));
        return;
    }

    for (int32 i = 0; i < 4; i++)
    {
        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocations[i]);
        SpawnTransform.SetLocation(SpawnLocations[i]);
        // 회전값은 SpawnRotations 배열의 값을 사용함 (있으면)
        if (SpawnRotations.IsValidIndex(i))
        {
            SpawnTransform.SetRotation(SpawnRotations[i].Quaternion());
        }
        else
        {
            SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
        }
        ABaseCharacter* NewCharacter = World->SpawnActor<ABaseCharacter>(DefaultPawnClass, SpawnTransform);
        if (NewCharacter)
        {
            SpawnedPlayers.Add(NewCharacter);
            UE_LOG(LogTemp, Log, TEXT("BattleGameMode: 플레이어 %d 스폰됨, 위치: %s"), i, *SpawnLocations[i].ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: 플레이어 %d 스폰 실패"), i);
        }
    }

    // PossessIndex 안전 검사 후, 해당 인덱스의 캐릭터를 소유하도록 함
    if (SpawnedPlayers.IsValidIndex(PossessIndex))
    {
        PC = UGameplayStatics::GetPlayerController(World, 0);
        if (PC)
        {
            PC->Possess(SpawnedPlayers[PossessIndex]);
            UE_LOG(LogTemp, Log, TEXT("BattleGameMode: PlayerController가 인덱스 %d의 캐릭터를 소유함."), PossessIndex);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BattleGameMode: PossessIndex %d가 유효하지 않음."), PossessIndex);
    }
    
    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));  
    if (MyGameInstance)
    {
        // 1초마다 경과시간 호출 함수 타이머 설정
        // GetWorld()->GetTimerManager().SetTimer(battle_timer_handle, this, &ABattleGameMode::PrintElapsedtime, 1.0f, true); 
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
