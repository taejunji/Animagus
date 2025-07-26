// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Server/Server/protocol.h"
#include "BaseItem.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;
class USoundAttenuation;

UCLASS(Abstract, Blueprintable)
class PROJECT_ANIMAGUS_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 아이템의 충돌 판정을 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    USphereComponent* CollisionComp;

    /** 아이템 획득 시 재생할 피크업 이펙트*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UNiagaraSystem* SpawnEffect;

    /** 아이템 획득 시 재생할 피크업 이펙트 (나이아가라 시스템) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UNiagaraSystem* PickupEffect;
    
    // 아이템이 월드에 있을 때 보이는 시각 
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    UNiagaraComponent* ItemEffect;

    UPROPERTY(EditAnywhere, Category="Audio")
    USoundBase* PickSound;

    UPROPERTY(EditAnywhere, Category="Audio")
    USoundAttenuation* AttenuationSettings;

    // 아이템이 이미 획득되었는지 판별하는 플래그
    bool bIsPickedUp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class UAIPerceptionStimuliSourceComponent* StimuliSource;

    /** 충돌 이벤트: 다른 액터와 Overlap 시 호출 */
    UFUNCTION()
    virtual void OnItemOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                    bool bFromSweep, const FHitResult & SweepResult);
    
public:
    // 아이템 획득 시 호출되는 함수.
    
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual void OnPickedUp(class ABaseCharacter* Picker);

    /** 아이템 소멸 및 정리 */
    UFUNCTION(BlueprintCallable, Category = "Item")
    virtual void DestroyItem();
    
    void SendItemPickedUp2Server(ABaseCharacter* Picker);

public:
    void SetItemType(Protocol::ItemType NewItemType) { ItemType = NewItemType; }
    Protocol::ItemType GetItemType() const { return ItemType; }

    Protocol::ItemType ItemType = Protocol::ItemType::NONE; // 아이템 타입 (기본값은 NONE)
};
