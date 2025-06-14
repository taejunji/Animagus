#include "MeshSelectWidget.h"
#include "Components/Button.h"
#include "../PlayerController/ConnectPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Animagus/PlayerController/ConnectPlayerController.h"
#include "Project_Animagus/System/MyGameInstance.h"

void UMeshSelectWidget::SetupOwner(AConnectPlayerController* InController)
{
    Owner = InController;
}

void UMeshSelectWidget::ActiveStartButton()
{
    if (BtnStart)
    {
        BtnStart->SetIsEnabled(true);
        BtnStart->SetVisibility(ESlateVisibility::Visible);
    }
}

void UMeshSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();


    if (BtnBack)
    {
        BtnBack->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleBackClicked);
        BtnBack->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnBack->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound); 
    }
    if (BtnNext)
    {
    BtnNext->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleNextClicked);
        BtnNext->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnNext->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound); 
    }

 // Monkey
    if (BtnMonkey)
    {
        BtnMonkey->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnMonkey->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnMonkey->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectMonkey);
    }

    // Koala
    if (BtnKoala)
    {
        BtnKoala->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnKoala->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnKoala->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectKoala);
    }

    // Sheep
    if (BtnSheep)
    {
        BtnSheep->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnSheep->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnSheep->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectSheep);
    }

    // Fox
    if (BtnFox)
    {
        BtnFox->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnFox->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnFox->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectFox);
    }

    // Sloth
    if (BtnSloth)
    {
        BtnSloth->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnSloth->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnSloth->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectSloth);
    }

    // Elephant
    if (BtnElephant)
    {
        BtnElephant->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnElephant->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnElephant->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectElephant);
    }

    // Raccoon
    if (BtnRaccoon)
    {
        BtnRaccoon->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnRaccoon->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnRaccoon->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectRaccoon);
    }

    // Deer
    if (BtnDeer)
    {
        BtnDeer->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnDeer->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnDeer->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectDeer);
    }

    // Cow
    if (BtnCow)
    {
        BtnCow->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnCow->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnCow->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectCow);
    }

    // Unicorn
    if (BtnUnicorn)
    {
        BtnUnicorn->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnUnicorn->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnUnicorn->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectUnicorn);
    }

    // Zebra
    if (BtnZebra)
    {
        BtnZebra->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnZebra->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnZebra->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectZebra);
    }

    // Donkey
    if (BtnDonkey)
    {
        BtnDonkey->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnDonkey->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnDonkey->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleSelectDonkey);
    }

    if (BtnStart)
    {
        BtnStart->OnHovered.AddDynamic(this, &UMeshSelectWidget::PlayHoverSound);
        BtnStart->OnClicked.AddDynamic(this, &UMeshSelectWidget::PlayClickSound);
        BtnStart->OnClicked.AddDynamic(this, &UMeshSelectWidget::HandleStartClicked);
    }

    BtnStart->SetVisibility(ESlateVisibility::Collapsed);
}

void UMeshSelectWidget::PlayHoverSound()
{
    if (HoverSound)
        UGameplayStatics::PlaySound2D(this, HoverSound);
}

void UMeshSelectWidget::PlayClickSound()
{
    if (ClickSound)
        UGameplayStatics::PlaySound2D(this, ClickSound);
}

void UMeshSelectWidget::HandleSelectMonkey()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Monkey); }

void UMeshSelectWidget::HandleSelectKoala()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Koala); }

void UMeshSelectWidget::HandleSelectSheep()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Sheep); }

void UMeshSelectWidget::HandleSelectFox()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Fox); }

void UMeshSelectWidget::HandleSelectSloth()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Sloth); }

void UMeshSelectWidget::HandleSelectElephant()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Elephant); }

void UMeshSelectWidget::HandleSelectRaccoon()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Raccoon); }

void UMeshSelectWidget::HandleSelectDeer()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Deer); }

void UMeshSelectWidget::HandleSelectCow()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Cow); }

void UMeshSelectWidget::HandleSelectUnicorn()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Unicorn); }

void UMeshSelectWidget::HandleSelectZebra()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Zebra); }

void UMeshSelectWidget::HandleSelectDonkey()
{ if (Owner.IsValid()) Owner->OnMeshSelected(CharacterMesh::Donkey); }

void UMeshSelectWidget::HandleBackClicked()
{
    if (!Owner.IsValid()) return;

    const int32 N = Owner->MeshList.Num();
    Owner->CurrentMeshIndex = (Owner->CurrentMeshIndex - 1 + N) % N;

    // 컨트롤러에 선택 알림
    Owner->OnMeshSelected(Owner->MeshList[Owner->CurrentMeshIndex]);
}

void UMeshSelectWidget::HandleNextClicked()
{
    if (!Owner.IsValid()) return;
    const int32 N = Owner->MeshList.Num();
    Owner->CurrentMeshIndex = (Owner->CurrentMeshIndex + 1) % N;
    Owner->OnMeshSelected(Owner->MeshList[Owner->CurrentMeshIndex]);
    
}

void UMeshSelectWidget::HandleStartClicked()
{
    if (Owner.IsValid())
        Owner->OnStartGame();
}
