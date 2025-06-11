#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../System/MyGameInstance.h"        
#include "ConnectPlayerController.generated.h"

class UMeshSelectWidget;
class ASkeletalPreviewActor;
class UAnimSequence;

UCLASS()
class PROJECT_ANIMAGUS_API AConnectPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    void ShowConnectUI();
    void OnMeshSelected(CharacterMesh Selected);  
    void OnStartGame();

protected:
    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<UMeshSelectWidget> MeshSelectWidgetClass;
    

    UPROPERTY(EditAnywhere, Category="Preview")
    UAnimSequence* IdleAnimation;

private:
    UPROPERTY()
    UMeshSelectWidget* MeshSelectWidget;

    UPROPERTY()
    ASkeletalPreviewActor* PreviewActor = nullptr;

    static FString MeshKeyFromEnum(CharacterMesh Mesh)
    {
        switch (Mesh)
        {
        case CharacterMesh::Monkey:   return TEXT("SM_Monkey");
        case CharacterMesh::Koala:    return TEXT("SM_Koala");
        case CharacterMesh::Sheep:    return TEXT("SM_Sheep");
        case CharacterMesh::Fox:      return TEXT("SM_Fox");
        case CharacterMesh::Sloth:    return TEXT("SM_Sloth");
        case CharacterMesh::Elephant: return TEXT("SM_Elephant");
        case CharacterMesh::Raccoon:  return TEXT("SM_Raccoon");
        case CharacterMesh::Deer:     return TEXT("SM_Deer");
        case CharacterMesh::Cow:      return TEXT("SM_Cow");
        case CharacterMesh::Unicorn:  return TEXT("SM_Unicorn");
        case CharacterMesh::Zebra:    return TEXT("SM_Zebra");
        case CharacterMesh::Donkey:   return TEXT("SM_Donkey");
        }
        return TEXT("");
    }
};
