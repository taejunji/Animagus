// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginPlayerController.h"
#include "../UI/LoginWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "../System/MyGameInstance.h"
#include "../Server/Server/protocol.h"
#include "../Network/Session.h"
#include "../Network/ClientPacketHandler.h"


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

    FTCHARToUTF8 IDConverter(*UserID);
    const char* IdString = IDConverter.Get();
    uint16 IdLen = IDConverter.Length();

    FTCHARToUTF8 PwdConverter(*Password);
    const char* PwdString = PwdConverter.Get();
    uint16 PwdLen = PwdConverter.Length();

    if (IdLen > Protocol::MAX_NAME_LEN || PwdLen > Protocol::MAX_NAME_LEN) {

    }

    Protocol::CS_LOGIN_PKT LoginPkt;
    ::strcpy_s(LoginPkt.login_id, IdString);
    ::strcpy_s(LoginPkt.login_pwd, PwdString);
    LoginPkt.id_len = IdLen;
    LoginPkt.pwd_len = PwdLen;

    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(LoginPkt);
    Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);

    
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