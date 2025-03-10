#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JumpMushroom.generated.h"

/**
 * JumpMushroom
 * 캐릭터가 밟으면 평소 점프보다 강한 임펄스를 주는 점프 버섯 액터.
 * 물리 충돌용 박스와 점프 효과를 위한 트리거 박스를 별도로 사용함.
 */
UCLASS()
class PROJECT_ANIMAGUS_API AJumpMushroom : public AActor
{
    GENERATED_BODY()
    
public:	
    AJumpMushroom();

protected:
    virtual void BeginPlay() override;

    // 물리 충돌용 박스. 실제 충돌 처리는 이 컴포넌트가 담당함.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    class UBoxComponent* PhysicalCollisionVolume;

    // 점프 효과를 발생시키기 위한 트리거용 박스.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    class UBoxComponent* JumpTriggerVolume;

    // 시각적 표현을 위한 메쉬.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    class UStaticMeshComponent* Mesh;

    // 점프 임펄스 값.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="JumpMushroom")
    FVector JumpImpulse;

    // 트리거 박스 오버랩 이벤트 핸들러.
    UFUNCTION()
    void OnTriggerOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                               class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                               bool bFromSweep, const FHitResult & SweepResult);
};
