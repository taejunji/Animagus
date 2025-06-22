// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginPlayerController.h"
#include "../UI/LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void ALoginPlayerController::BeginPlay()
{
    Super::BeginPlay();
    ShowLoginUI();
}

void ALoginPlayerController::ShowLoginUI()
{
    if (!LoginWidgetClass) 
        return;

    LoginWidget = CreateWidget<ULoginWidget>(this, LoginWidgetClass);
    if (!LoginWidget) 
        return;

    LoginWidget->AddToViewport();
    LoginWidget->SetupOwner(this);
    LoginWidget->BindButtons();

    // UI 전용 입력 모드
    FInputModeUIOnly Mode;
    Mode.SetWidgetToFocus(LoginWidget->TakeWidget());
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    SetInputMode(Mode);
    bShowMouseCursor = true;
}

void ALoginPlayerController::OnLoginClicked()
{
    if (!LoginWidget) return;

    // 텍스트 박스에서 아이디/비번 읽기
    const FString UserID   = LoginWidget->TxtUserID->GetText().ToString();
    const FString Password = LoginWidget->TxtPassword->GetText().ToString();





    
    // 여기서 서버 로그인 요청 및 openlevel
    // UserId, Password
    





    
    // 확인용
    LoginWidget->ShowResult(FText::FromString(TEXT("아이디 확인 실패")));

    FTimerHandle UnusedHandle;
    GetWorldTimerManager().SetTimer(
        UnusedHandle,
        [this]()
        {
            if (LoginWidget)
                LoginWidget->HideResult();
        },
        2.0f,  
        false  // 한번만
    );
    
}

void ALoginPlayerController::OnSignupClicked()
{
    if (!LoginWidget) return;

    // 텍스트 박스에서 아이디/비번 읽기
    const FString UserID   = LoginWidget->TxtUserID->GetText().ToString();
    const FString Password = LoginWidget->TxtPassword->GetText().ToString();







    
    // 여기서 서버 회원가입 요청
    // UserId, Password 








    
    // 확인용
    LoginWidget->ShowResult(FText::FromString(TEXT("생성 완료")));

    FTimerHandle UnusedHandle;
    GetWorldTimerManager().SetTimer(
        UnusedHandle,
        [this]()
        {
            if (LoginWidget)
                LoginWidget->HideResult();
        },
        2.0f,  
        false  // 한번만
    ); 
}