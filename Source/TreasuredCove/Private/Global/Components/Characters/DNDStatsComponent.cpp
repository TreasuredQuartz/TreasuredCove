// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Characters/DNDStatsComponent.h"

// Sets default values for this component's properties
UDNDStatsComponent::UDNDStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDNDStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


/* Called every frame
void UDNDStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
} // */

