// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickup);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDropped);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UPickupComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	// Whether the item is on the ground or a character has it on their person or in their inventory.
	uint8 bPickedUp : 1;

public:	
	// Sets default values for this component's properties
	UPickupComponent();

	// Fires off a delegate so the owner can react to being picked up
	void Pickup();
	// Fires off a delegate so the owner can react to being dropped.
	void Drop();

	// For State Checking
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool IsPickedUp() const { return bPickedUp; };

public:
	UPROPERTY(BlueprintAssignable)
	FOnPickup OnPickup;
	UPROPERTY(BlueprintAssignable)
	FOnDropped OnDrop;
};
