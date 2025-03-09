#include "JumpMushroom.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AJumpMushroom::AJumpMushroom()
{
    PrimaryActorTick.bCanEverTick = false;

    // 생성: 물리 충돌용 박스 생성 및 설정
    PhysicalCollisionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("PhysicalCollisionVolume"));
    RootComponent = PhysicalCollisionVolume;
    PhysicalCollisionVolume->SetCollisionProfileName(TEXT("BlockAll"));

    // 생성: 시각적 메쉬 생성, 물리 충돌은 PhysicalCollisionVolume로 처리하기 위해 NoCollision 설정
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 생성: 점프 효과 트리거용 박스 생성 및 설정
    JumpTriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("JumpTriggerVolume"));
    JumpTriggerVolume->SetupAttachment(RootComponent);
    JumpTriggerVolume->SetCollisionProfileName(TEXT("Trigger"));
    // 트리거 박스의 상대 위치와 크기 설정 (필요에 따라 조정 가능)
    JumpTriggerVolume->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
    JumpTriggerVolume->SetBoxExtent(FVector(50.f, 50.f, 20.f));

    // 트리거 박스 오버랩 이벤트 바인딩
    JumpTriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AJumpMushroom::OnTriggerOverlapBegin);

    // 기본 점프 임펄스 값 설정 (Z값가 높게 설정)
    JumpImpulse = FVector(0.f, 0.f, 1200.f);
}

void AJumpMushroom::BeginPlay()
{
    Super::BeginPlay();
}

void AJumpMushroom::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                          bool bFromSweep, const FHitResult & SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        ACharacter* Character = Cast<ACharacter>(OtherActor);
        if (Character)
        {
            Character->LaunchCharacter(JumpImpulse, true, true);
            UE_LOG(LogTemp, Log, TEXT("JumpMushroom: %s triggered jump with impulse %s"), *Character->GetName(), *JumpImpulse.ToString());
        }
    }
}
