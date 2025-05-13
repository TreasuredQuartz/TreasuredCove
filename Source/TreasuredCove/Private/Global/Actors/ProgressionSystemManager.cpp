// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Actors/ProgressionSystemManager.h"

// Sets default values
AProgressionSystemManager::AProgressionSystemManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProgressionSystemManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProgressionSystemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

