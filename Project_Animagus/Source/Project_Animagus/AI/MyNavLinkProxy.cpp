// Fill out your copyright notice in the Description page of Project Settings.


#include "MyNavLinkProxy.h"
#include "MyAIController.h"
#include "../Character/AICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AMyNavLinkProxy::AMyNavLinkProxy()
{
}

void AMyNavLinkProxy::OnSmartLinkReached(AActor* MovingActor)
{
    AAICharacter* AI = Cast<AAICharacter>(MovingActor);
    if (AI == nullptr) return;

    AI->Jump(); 
}
