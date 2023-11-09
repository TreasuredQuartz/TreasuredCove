// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Actors/LootSystemManager.h"

// Sets default values
ALootSystemManager::ALootSystemManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALootSystemManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALootSystemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

