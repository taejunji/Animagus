// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UConnectWidget.generated.h"

class UEditableTextBox;
class UButton;

/**
 * 
 */
UCLASS()
class PROJECT_ANIMAGUS_API UUConnectWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** 에디터에서 배치한 텍스트 박스 (아이디: InputIP) */
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* InputIP;

    /** 에디터에서 배치한 버튼 (아이디: Connect_Button) */
    UPROPERTY(meta = (BindWidget, AllowPrivateAccess = true))
    UButton* ConnectButton;
	
};
