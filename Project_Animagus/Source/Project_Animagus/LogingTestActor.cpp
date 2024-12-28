// Fill out your copyright notice in the Description page of Project Settings.


#include "LogingTestActor.h"

// Sets default values
ALogingTestActor::ALogingTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALogingTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALogingTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Display, TEXT("ALogingTestActor Testing!"));

	// 한글 테스트
	// 한글 테스트2
}

