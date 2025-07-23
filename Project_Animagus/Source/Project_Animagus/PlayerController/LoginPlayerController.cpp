// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginPlayerController.h"
#include "../UI/LoginWidget.h"
#include "../UI/SignUpWidget.h"
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

    if (!SignUpWidgetClass) {
        UE_LOG(LogTemp, Warning, TEXT("BP 클래스 오류"));
        return;
    }

    SignUpWidget = CreateWidget<USignUpWidget>(this, SignUpWidgetClass);
    if (!SignUpWidget) {
        UE_LOG(LogTemp, Warning, TEXT("위젯 클래스 오류"));
        return;
    }

    SignUpWidget->AddToViewport();
    SignUpWidget->SetupOwner(this);
    SignUpWidget->BindButtons();
    SignUpWidget->SetVisibility(ESlateVisibility::Collapsed);

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

    if (IdLen > Protocol::MAX_NAME_LEN || IdLen == 0 || PwdLen > Protocol::MAX_NAME_LEN || PwdLen == 0) {
        // 확인용
        LoginWidget->ShowResult(FText::FromString(TEXT("글자 수 오류 (최대 20자)")));

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

        return;
    }

    Protocol::CS_LOGIN_PKT LoginPkt;
    ::strcpy_s(LoginPkt.login_id, IdString);
    ::strcpy_s(LoginPkt.login_pwd, PwdString);
    LoginPkt.id_len = IdLen;
    LoginPkt.pwd_len = PwdLen;

    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(LoginPkt);
    Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
}

void ALoginPlayerController::OnSignupClicked()
{
    if (!LoginWidget) return;
    //if (!SignUpWidget) return;

    UE_LOG(LogTemp, Warning, TEXT("SignUpClicked"));

    SignUpWidget->SetVisibility(ESlateVisibility::Visible);

    //// 텍스트 박스에서 아이디/비번 읽기
    //const FString UserID   = LoginWidget->TxtUserID->GetText().ToString();
    //const FString Password = LoginWidget->TxtPassword->GetText().ToString();
    //// 확인용
    //LoginWidget->ShowResult(FText::FromString(TEXT("생성 완료")));

    //FTimerHandle UnusedHandle;
    //GetWorldTimerManager().SetTimer(
    //    UnusedHandle,
    //    [this]()
    //    {
    //        if (LoginWidget)
    //            LoginWidget->HideResult();
    //    },
    //    2.0f,  
    //    false  // 한번만
    //); 
    
    // TODO: SignUp 위젯 띄우기
}

void ALoginPlayerController::OnSignOkClicked()
{
    if (!SignUpWidget) return;
    if (!LoginWidget) return;
    UE_LOG(LogTemp, Warning, TEXT("SignOkClicked"));

    // 텍스트 박스에서 아이디/비번 읽기
    const FString UserID = SignUpWidget->TxtUserID->GetText().ToString();
    const FString Password = SignUpWidget->TxtPassword->GetText().ToString();
    const FString NickName = SignUpWidget->TxtNickName->GetText().ToString();

    FTCHARToUTF8 IDConverter(*UserID);
    const char* IdString = IDConverter.Get();
    uint16 IdLen = IDConverter.Length();

    FTCHARToUTF8 PwdConverter(*Password);
    const char* PwdString = PwdConverter.Get();
    uint16 PwdLen = PwdConverter.Length();

    FTCHARToUTF8 NameConverter(*NickName);
    const char* NameString = NameConverter.Get();
    uint16 NameLen = NameConverter.Length();

    if (IdLen > Protocol::MAX_NAME_LEN || IdLen == 0 
        || PwdLen > Protocol::MAX_NAME_LEN || PwdLen == 0
        || NameLen > Protocol::MAX_NAME_LEN || NameLen == 0)
    {
        //SignUpWidget->ShowResult(FText::FromString(TEXT("글자 수 오류 (최대 20자)")));

        //FTimerHandle UnusedHandle;
        //GetWorldTimerManager().SetTimer(
        //    UnusedHandle,
        //    [this]()
        //    {
        //        if (SignUpWidget)
        //            SignUpWidget->HideResult();
        //    },
        //    2.0f,
        //    false  // 한번만
        //);

        return;
    }

    Protocol::CS_SIGN_UP_PKT SignUpPkt;
    ::strcpy_s(SignUpPkt.sign_id, IdString);
    ::strcpy_s(SignUpPkt.sign_pwd, PwdString);
    ::strcpy_s(SignUpPkt.sign_name, NameString);

    SignUpPkt.id_len = IdLen;
    SignUpPkt.pwd_len = PwdLen;
    SignUpPkt.name_len = NameLen;

    SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(SignUpPkt);
    Cast<UMyGameInstance>(GWorld->GetGameInstance())->SendPacket(SendBuffer);
}

void ALoginPlayerController::OnSignCancelClicked()
{
    if (!SignUpWidget) return;
    if (!LoginWidget) return;
    UE_LOG(LogTemp, Warning, TEXT("SignCancleClicked"));

    SignUpWidget->ClearInputs();
    SignUpWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ALoginPlayerController::HandleLoginSuccess()
{
    if (nullptr == LoginWidget) return;
    LoginWidget->ShowResult(FText::FromString(TEXT("로그인 성공!")));;
    FTimerHandle UnusedHandle;
    GetWorldTimerManager().SetTimer(
        UnusedHandle,
        [this]()
        {
            if (LoginWidget)
                LoginWidget->HideResult();
        },
        1.0f,
        false  // 한번만
    );
}

void ALoginPlayerController::HandleLoginFail(Protocol::SC_LOGIN_FAIL_PKT& pkt)
{
    if (nullptr == LoginWidget) return;
    switch (pkt.reason)
    {
    case Protocol::LOGIN_ERR:
        LoginWidget->ShowResult(FText::FromString(TEXT("잘못된 입력입니다.")));
        break;
    case Protocol::LOGIN_NOEX:
        LoginWidget->ShowResult(FText::FromString(TEXT("아이디 또는 비밀번호가 일치하지 않습니다.")));
        break;
    case Protocol::LOGIN_USING:
        LoginWidget->ShowResult(FText::FromString(TEXT("이미 로그인된 계정입니다.")));
        break;
    default:
        break;
    }
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

void ALoginPlayerController::HandleSignUpSuccess()
{
    SignUpWidget->ClearInputs();
    SignUpWidget->SetVisibility(ESlateVisibility::Collapsed);

    LoginWidget->ShowResult(FText::FromString(TEXT("회원가입 완료")));

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

void ALoginPlayerController::HandleSignUpFail()
{
    SignUpWidget->ShowResult(FText::FromString(TEXT("회원가입 실패 (중복ID)")));

    FTimerHandle UnusedHandle;
    GetWorldTimerManager().SetTimer(
        UnusedHandle,
        [this]()
        {
            if (SignUpWidget)
                SignUpWidget->HideResult();
        },
        2.0f,
        false  // 한번만
    );
}
