// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Framework/GAGameClock.h"

// Sets default values
UGAGameClock::UGAGameClock()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts or when spawned
void UGAGameClock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void UGAGameClock::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



