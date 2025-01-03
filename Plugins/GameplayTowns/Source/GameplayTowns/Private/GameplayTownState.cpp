// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTownState.h"
#include "Library/GameplayLibrary.h"
#include "GameplayClock.h"

// Sets default values
AGameplayTownState::AGameplayTownState()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ClockComponent = 
		CreateDefaultSubobject<UGameplayClockComponent>(TEXT("Town Clock"));
}

// Called when the game starts or when spawned
void AGameplayTownState::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameplayTownState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameplayTownState::SetTown(AGameplayTown* InTown)
{
	Town = InTown;
}

