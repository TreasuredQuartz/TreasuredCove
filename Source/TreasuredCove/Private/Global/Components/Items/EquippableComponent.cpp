// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/EquippableComponent.h"

// Sets default values for this component's properties
UEquippableComponent::UEquippableComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEquippableComponent::Equipped()
{
	bEquipped = true;

	OnEquipped.Broadcast();
}

void UEquippableComponent::UnEquipped()
{
	bEquipped = false;

	OnUnEquipped.Broadcast();
}

