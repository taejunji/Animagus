#include "ConnectPlayerController.h"
#include "../UI/MeshSelectWidget.h"
#include "../Actor/SkeletalPreviewActor.h"
#include "../System/MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/GameMode/ConnectGameMode.h"
#include "Components/SkeletalMeshComponent.h"

void AConnectPlayerController::BeginPlay()
{
    Super::BeginPlay();

    ShowConnectUI();

    if (AConnectGameMode* GM = Cast<AConnectGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        PreviewActor = GM->GetPreviewActor();
    }

    MeshList = {
        CharacterMesh::Sheep,
        CharacterMesh::Monkey,
        CharacterMesh::Koala,
        CharacterMesh::Fox,
        CharacterMesh::Sloth,
        CharacterMesh::Elephant,
        CharacterMesh::Raccoon,
        CharacterMesh::Deer,
        CharacterMesh::Cow,
        CharacterMesh::Unicorn,
        CharacterMesh::Zebra,
        CharacterMesh::Donkey
    };

    if (MeshList.Num() > 0)
    {
        CurrentMeshIndex = 0;
        OnMeshSelected(MeshList[0]);
    }
    
    if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
        OnMeshSelected(GI->player_data.stored_mesh);
}

void AConnectPlayerController::ShowConnectUI()
{
    if (!MeshSelectWidgetClass) return;

    FInputModeGameAndUI Mode;

//    FInputModeUIOnly Mode;
    Mode.SetWidgetToFocus(nullptr);
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
    bShowMouseCursor = true;
    
    MeshSelectWidget = CreateWidget<UMeshSelectWidget>(this, MeshSelectWidgetClass);
    MeshSelectWidget->AddToViewport();
    MeshSelectWidget->SetupOwner(this);
}

void AConnectPlayerController::OnMeshSelected(CharacterMesh Selected)
{

    if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
    {
        GI->player_data.stored_mesh = Selected;

        // 문자열 키 변환
        const FString Key = MeshKeyFromEnum(Selected);
        if (USkeletalMesh** MeshPtr = GI->CharacterMeshes.Find(Key))
        {
            if (PreviewActor)
            {
                // 이제 SetMesh만 호출: Idle만 재생
                PreviewActor->SetMesh(*MeshPtr);
            }
        }
    }
    
    // UE_LOG(LogTemp, Warning, TEXT("OnMeshSelected called with %d"), (int32)Selected);
    //
    // if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
    // {
    //     // 1) 저장
    //     if (!GI)
    //     {
    //         UE_LOG(LogTemp, Error, TEXT("OnMeshSelected: GameInstance is null"));
    //         return;
    //     }
    //     GI->player_data.stored_mesh = Selected;
    //
    //     // 2) PreviewActor 확인
    //     if (!PreviewActor)
    //     {
    //         UE_LOG(LogTemp, Error, TEXT("OnMeshSelected: PreviewActor is null"));
    //         return;
    //     }
    //
    //     
    //     // 2) 맵에서 문자열 키로 찾기
    //     FString Key = MeshKeyFromEnum(Selected);
    //     UE_LOG(LogTemp, Log, TEXT("OnMeshSelected: Key = %s"), *Key);
    //     
    //     // 4) 메쉬 검색
    //     USkeletalMesh** MeshPtr = GI->CharacterMeshes.Find(Key);
    //     if (!MeshPtr)
    //     {
    //         UE_LOG(LogTemp, Error, TEXT("OnMeshSelected: CharacterMeshes has no entry for %s"), *Key);
    //         return;
    //     }
    //     UE_LOG(LogTemp, Log, TEXT("OnMeshSelected: Found mesh ptr %p"), MeshPtr);
    //     
    //     PreviewActor->SetMesh(*MeshPtr);
    //     UE_LOG(LogTemp, Log, TEXT("OnMeshSelected: SetMeshAndPlay called"));
    // }
}

void AConnectPlayerController::OnStartGame()
{
    // 여기가  


    
    UGameplayStatics::OpenLevel(this, TEXT("L_Map"));
}

void AConnectPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (!InPawn) return;

    // 1) Pawn 위치·회전 강제
    InPawn->SetActorLocationAndRotation(
      InitialPawnLocation,
      InitialPawnRotation ,
      false, nullptr,
      ETeleportType::TeleportPhysics
    );

    // 2) 컨트롤러 회전 강제
    SetControlRotation(InitialPawnRotation );
}

