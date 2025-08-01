// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomSelectWidget.h"
#include "Components/TextBlock.h"
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
    
    InitMessage = FText::FromString(
        TEXT(
            "조금 전에 보신\n"
            "애니마구스의 인트로 영상은\n"
            "김현 교수님이 담당하시는\n"
            "3D 애니메이션2 교과목에서\n"
            "제작한 거예요!\n\n"
            "강의 많이 수강해 주세요!"
        ));

    if (!DragonText) return;
    DragonText->SetText(InitMessage);
    DragonText->SetVisibility(ESlateVisibility::Visible);
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

void URoomSelectWidget::ShowResult(const FText& Message)
{
    if (!DragonText) return;
    DragonText->SetText(Message);
}
