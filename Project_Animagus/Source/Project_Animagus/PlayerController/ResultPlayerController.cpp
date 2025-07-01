// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultPlayerController.h"

#include "EngineUtils.h"
#include "NiagaraFunctionLibrary.h"
#include "../UI/ResultWidget.h"
#include "../System/MyGameInstance.h"
#include "Camera/CameraActor.h"
#include "Components/LightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/Actor/ResultCharacterActor.h"

void AResultPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    if (!ResultWidgetClass) return;

    ResultWidget = CreateWidget<UResultWidget>(this, ResultWidgetClass);
    if (!ResultWidget) return;

    ResultWidget->AddToViewport();
    bShowMouseCursor = true;
    
    if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
    {
        bIsWin = (GI->myRank == 1);
        ResultWidget->ShowOutcome(bIsWin);
    }

    if (ResultActorClass)
    {
        AResultCharacterActor* RA = GetWorld()->SpawnActor<AResultCharacterActor>(
            ResultActorClass,
            ResultActorSpawnTransform.GetLocation(),
            ResultActorSpawnTransform.GetRotation().Rotator()
        );
        if (RA)
        {
            USkeletalMesh* ChosenMesh = nullptr;

            if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
            {

                FString Key;
                switch (GI->player_data.stored_mesh)
                {
                case CharacterMesh::Monkey:   Key = TEXT("SM_Monkey");    break;
                case CharacterMesh::Koala:    Key = TEXT("SM_Koala");     break;
                case CharacterMesh::Sheep:    Key = TEXT("SM_Sheep");     break;
                case CharacterMesh::Fox:      Key = TEXT("SM_Fox");       break;
                case CharacterMesh::Sloth:    Key = TEXT("SM_Sloth");     break;
                case CharacterMesh::Elephant: Key = TEXT("SM_Elephant");  break;
                case CharacterMesh::Raccoon:  Key = TEXT("SM_Raccoon");   break;
                case CharacterMesh::Deer:     Key = TEXT("SM_Deer");      break;
                case CharacterMesh::Cow:      Key = TEXT("SM_Cow");       break;
                case CharacterMesh::Unicorn:  Key = TEXT("SM_Unicorn");   break;
                case CharacterMesh::Zebra:    Key = TEXT("SM_Zebra");     break;
                case CharacterMesh::Donkey:   Key = TEXT("SM_Donkey");    break;
                default:                      Key = TEXT("");              break;
                }

                if (!Key.IsEmpty())
                {
                    if (USkeletalMesh** MeshPtr = GI->CharacterMeshes.Find(Key))
                    {
                        ChosenMesh = *MeshPtr;
                    }
                }
            }

            // 3) 초기화 호출
            UAnimSequence* OutcomeAnim = bIsWin ? WinnerAnimation : LoserAnimation;
            RA->Initialize(ChosenMesh, OutcomeAnim);
        }
    }

    
        USoundBase* ToPlay = bIsWin ? WinnerBGM : LoserBGM;
        if (ToPlay)
            UGameplayStatics::PlaySound2D(this, ToPlay);
   

    if (bIsWin && ConfettiFX)
    {
        for (const FTransform& T : ConfettiSpawnTransforms)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                this,
                ConfettiFX,
                T.GetLocation(),
                T.GetRotation().Rotator(),
                T.GetScale3D()
            );
        }
    }

    if (!bIsWin && LostFX)
    {
        for (const FTransform& Tfm : LostFXSpawnTransforms)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                this,
                LostFX,
                Tfm.GetLocation(),
                Tfm.GetRotation().Rotator(),
                Tfm.GetScale3D()
            );
        }

        // 라이트 조절
        for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
        {
            ADirectionalLight* DirLight = *It;
            if (DirLight && DirLight->GetLightComponent())
            {
                // 강도
                DirLight->GetLightComponent()->SetIntensity(LostLightIntensity);
                // 색상
                DirLight->GetLightComponent()->SetLightColor(LostLightColor);
            }
        }
    }
    
    for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
    {
        ACameraActor* Cam = *It;
        if (Cam->ActorHasTag(TEXT("ResultCamera")))
        {
            SetViewTargetWithBlend(Cam, 0.0f); // 즉시 전환
            break;
        }
    }
}

