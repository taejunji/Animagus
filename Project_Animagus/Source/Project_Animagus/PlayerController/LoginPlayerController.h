// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LoginPlayerController.generated.h"

class ULoginWidget;
class USignUpWidget;

UCLASS()
class PROJECT_ANIMAGUS_API ALoginPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
    virtual void BeginPlay() override;

    // 로그인 UI 띄우기
    void ShowLoginUI();

    // Login 버튼 클릭
    UFUNCTION() void OnLoginClicked();

    // Signup 버튼 클릭
    UFUNCTION() void OnSignupClicked();

    // SignUp Ok 버튼 클릭
    UFUNCTION() void OnSignOkClicked();

    // SignUp Cancel 버튼 클릭
    UFUNCTION() void OnSignCancelClicked();


    
    // Widget Blueprint 클래스 지정
    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<ULoginWidget> LoginWidgetClass;
    
    UPROPERTY()
    ULoginWidget* LoginWidget;


    // Widget Blueprint 클래스 지정
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<USignUpWidget> SignUpWidgetClass;

    UPROPERTY()
    USignUpWidget* SignUpWidget;

};
