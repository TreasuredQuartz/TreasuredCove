// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/AbilitySystem/AttributeSets/Components/CharacteristicsComponent.h"

// Sets default values for this component's properties
UCharacteristicsComponent::UCharacteristicsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCharacteristicsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


/* Called every frame
void UCharacteristicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
} // */

