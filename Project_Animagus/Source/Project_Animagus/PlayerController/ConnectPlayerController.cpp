#include "ConnectPlayerController.h"
#include "../UI/MeshSelectWidget.h"
#include "../Actor/SkeletalPreviewActor.h"
#include "../System/MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/GameMode/ConnectGameMode.h"

void AConnectPlayerController::BeginPlay()
{
    Super::BeginPlay();

    ShowConnectUI();

    if (AConnectGameMode* GM = Cast<AConnectGameMode>(UGameplayStatics::GetGameMode(this)))
    {
        PreviewActor = GM->GetPreviewActor();
    }
    
}

void AConnectPlayerController::ShowConnectUI()
{
    if (!MeshSelectWidgetClass) return;

    FInputModeUIOnly Mode;
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
    UE_LOG(LogTemp, Warning, TEXT("OnMeshSelected called with %d"), (int32)Selected);
    
    if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
    {
        // 1) 저장
        if (!GI)
        {
            UE_LOG(LogTemp, Error, TEXT("OnMeshSelected: GameInstance is null"));
            return;
        }
        GI->player_data.stored_mesh = Selected;

        // 2) PreviewActor 확인
        if (!PreviewActor)
        {
            UE_LOG(LogTemp, Error, TEXT("OnMeshSelected: PreviewActor is null"));
            return;
        }

        
        // 2) 맵에서 문자열 키로 찾기
        FString Key = MeshKeyFromEnum(Selected);
        UE_LOG(LogTemp, Log, TEXT("OnMeshSelected: Key = %s"), *Key);
        
        // 4) 메쉬 검색
        USkeletalMesh** MeshPtr = GI->CharacterMeshes.Find(Key);
        if (!MeshPtr)
        {
            UE_LOG(LogTemp, Error, TEXT("OnMeshSelected: CharacterMeshes has no entry for %s"), *Key);
            return;
        }
        UE_LOG(LogTemp, Log, TEXT("OnMeshSelected: Found mesh ptr %p"), MeshPtr);

        // 5) PreviewActor에 설정
        PreviewActor->SetMeshAndPlay(*MeshPtr, IdleAnimation);
        UE_LOG(LogTemp, Log, TEXT("OnMeshSelected: SetMeshAndPlay called"));
    }
}

void AConnectPlayerController::OnStartGame()
{
    // 레벨 전환
    UGameplayStatics::OpenLevel(this, TEXT("L_Map"));
}

