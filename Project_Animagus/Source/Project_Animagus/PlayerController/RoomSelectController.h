// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LevelStreamingDynamic.h"
#include "RoomSelectController.generated.h"

class USoundBase;
class URoomSelectWidget;


UCLASS()
class PROJECT_ANIMAGUS_API ARoomSelectController : public APlayerController
{
	GENERATED_BODY()
  

public:
    virtual void BeginPlay() override;

    /** 위젯 생성 & 화면에 띄우기 */
    void ShowRoomSelectUI();

    void EnterRoom(uint8 roomIndex);
    void HandleRoomEnter();
    void HandleRoomEnterFail();

    
    /** 룸 선택 버튼 핸들러 */
    UFUNCTION() void OnRoom1Clicked();
    UFUNCTION() void OnRoom2Clicked();
    UFUNCTION() void OnRoom3Clicked();
    UFUNCTION() void OnRoom4Clicked();
    UFUNCTION() void OnRoom5Clicked();

    /** 버튼 사운드 재생 */
    UFUNCTION() void PlayHoverSound();
    UFUNCTION() void PlayClickSound();

protected:
    /** Widget Blueprint 에 지정할 클래스 */
    UPROPERTY(EditAnywhere, Category="UI")
    TSubclassOf<URoomSelectWidget> RoomSelectWidgetClass;

    /** 생성된 위젯 레퍼런스 */
    UPROPERTY()
    URoomSelectWidget* RoomSelectWidget;

    /** 버튼 호버/클릭 시 사용할 사운드 */
    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* HoverSound;

    UPROPERTY(EditAnywhere, Category="Sound")
    USoundBase* ClickSound;


};
