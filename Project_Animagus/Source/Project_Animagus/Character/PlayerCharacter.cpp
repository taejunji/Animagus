// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "../System//MyGameInstance.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 생성자에서 메쉬 설정하는법 -> 단점 [ 로비에서 설정한 메쉬를 적용할 때 코드가 귀찮아진다 ]
    //auto* MyMesh = GetMesh();
    //ConstructorHelpers::FObjectFinder<USkeletalMesh> FindMesh(TEXT("/Game/WorkFolder/Assets/Sheep/Character144_RiggedSheep.Character144_RiggedSheep"));
    //if (FindMesh.Succeeded())
    //{
    //    MyMesh->SetSkeletalMesh(FindMesh.Object);
    //    MyMesh->SetRelativeLocationAndRotation(FVector(0, 0, -60), FRotator(0, -90, 0));
    //}
    //else UE_LOG(LogTemp, Warning, TEXT("스켈레탈 로드 오류"));


    auto* MyCapsule = GetCapsuleComponent();
    MyCapsule->SetCapsuleSize(54.358692, 38.444557);

    spring_arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm")); 
    spring_arm->SetupAttachment(GetCapsuleComponent()); 
    spring_arm->TargetArmLength = 300.f; 
    spring_arm->SetRelativeLocationAndRotation(FVector(32, 0, -13), FRotator(-20, 0, 0)); // 카메라 위치, 회전 값 설정 - FRotator( Y, Z, X )축 회전  

    camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    camera->SetupAttachment(spring_arm);
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 생성자에서 메쉬를 초기화했을 때는 문제가 생겼음 -> GameInstance가 초기화되지 않았는데 포인터 접근해서 프로젝트가 터짐 -> 게임 시작 시( BeginPlay )에 메쉬를 설정하도록 수정
    // 에디터에서 메쉬가 없는것처럼 보이지만 정상동작이다.
    InitPlayerMesh();
    GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -55), FRotator(0, -90, 0)); // 메쉬 기본 위치, 회전값 설정
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void APlayerCharacter::InitPlayerMesh()
{
    if (auto* GameInstance = Cast<UMyGameInstance>(GetGameInstance()))
    {
        switch (GameInstance->player_data.stored_mesh)
        {
        case CharacterMesh::Monkey:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Monkey"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Monkey")]);
            break;

        case CharacterMesh::Tiger:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Tiger"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Tiger")]);
            break;

        case CharacterMesh::Koala:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Koala"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Koala")]);
            break;

        case CharacterMesh::Sheep:
            if (GameInstance->CharacterMeshes.Contains(TEXT("SM_Sheep"))) GetMesh()->SetSkeletalMesh(GameInstance->CharacterMeshes[TEXT("SM_Sheep")]);
            break;

        default:
            UE_LOG(LogTemp, Error, TEXT("메쉬 로드 실패"));
            break;
        }
    }
}