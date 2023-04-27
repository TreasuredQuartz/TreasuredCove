// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/FloatingBarActor.h"

// Sets default values
AFloatingBarActor::AFloatingBarActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFloatingBarActor::BeginPlay()
{
	Super::BeginPlay();
	
	AnchorLocation = GetActorLocation();
}

// Called every frame
void AFloatingBarActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
