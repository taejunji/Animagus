// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomSelectWidget.h"
#include "Components/Button.h"
#include "../PlayerController/RoomSelectController.h"


void URoomSelectWidget::SetupOwner(ARoomSelectController* InOwner)
{
    Owner = InOwner;
    BindAllButtons();
}

void URoomSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
}

void URoomSelectWidget::BindAllButtons()
{
    if (Owner.IsValid())
    {
        if (BtnRoom1)
        {
            BtnRoom1->OnHovered.AddDynamic(Owner.Get(), &ARoomSelectController::PlayHoverSound);
            BtnRoom1->OnClicked.AddDynamic(Owner.Get(), &ARoomSelectController::OnRoom1Clicked);
        }

        // Room 2
        if (BtnRoom2)
        {
            BtnRoom2->OnHovered.AddDynamic(Owner.Get(), &ARoomSelectController::PlayHoverSound);
            BtnRoom2->OnClicked.AddDynamic(Owner.Get(), &ARoomSelectController::OnRoom2Clicked);
        }

        // Room 3
        if (BtnRoom3)
        {
            BtnRoom3->OnHovered.AddDynamic(Owner.Get(), &ARoomSelectController::PlayHoverSound);
            BtnRoom3->OnClicked.AddDynamic(Owner.Get(), &ARoomSelectController::OnRoom3Clicked);
        }

        // Room 4
        if (BtnRoom4)
        {
            BtnRoom4->OnHovered.AddDynamic(Owner.Get(), &ARoomSelectController::PlayHoverSound);
            BtnRoom4->OnClicked.AddDynamic(Owner.Get(), &ARoomSelectController::OnRoom4Clicked);
        }

        // Room 5
        if (BtnRoom5)
        {
            BtnRoom5->OnHovered.AddDynamic(Owner.Get(), &ARoomSelectController::PlayHoverSound);
            BtnRoom5->OnClicked.AddDynamic(Owner.Get(), &ARoomSelectController::OnRoom5Clicked);
        }
    }
}
