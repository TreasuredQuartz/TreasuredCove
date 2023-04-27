// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/FloatingItemInfoActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFloatingItemInfoActor::AFloatingItemInfoActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bHasBegunPlay = false;
	AnchorLocation = FVector::ZeroVector;
}

// Called when the game starts or when spawned
void AFloatingItemInfoActor::BeginPlay()
{
	Super::BeginPlay();

	AnchorLocation = GetActorLocation();
	bHasBegunPlay = true;
}

// Called every frame
void AFloatingItemInfoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

