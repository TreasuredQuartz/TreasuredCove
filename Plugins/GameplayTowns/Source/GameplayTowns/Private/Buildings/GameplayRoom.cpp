// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/GameplayRoom.h"
#include "Buildings/GameplayBuilding.h"

// Sets default values for this component's properties
AGameplayRoom::AGameplayRoom()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void AGameplayRoom::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void AGameplayRoom::SetOwningBuilding(AGameplayBuilding* NewOwner)
{
	if (NewOwner)
	{
		OwningBuilding = NewOwner;
	}
	else
	{
		Destroy();
	}
}

