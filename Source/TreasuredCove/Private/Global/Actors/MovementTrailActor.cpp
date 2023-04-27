// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/MovementTrailActor.h"

// Sets default values
AMovementTrailActor::AMovementTrailActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMovementTrailActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
//void AMovementTrailActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

