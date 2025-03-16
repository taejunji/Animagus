// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "MyNavLinkProxy.generated.h"

/**
 * 네비 메쉬에 닿지 않는 곳을 Proxy를 두어 점프하도록 할 것이다
 */
UCLASS()
class PROJECT_ANIMAGUS_API AMyNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()
	
public:
    AMyNavLinkProxy();

    // 프록시가 도달했을 때 호출되는 함수
    UFUNCTION(BlueprintCallable, Category = "AI")
    void OnSmartLinkReached(AActor* MovingActor);
};
