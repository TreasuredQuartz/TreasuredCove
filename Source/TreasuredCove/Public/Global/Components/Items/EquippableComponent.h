// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquippableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquipped);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UEquippableComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FString EquipSlot;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	uint8 bEquipped : 1;

public:	
	// Sets default values for this component's properties
	UEquippableComponent();

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FString GetEquipSlot() const { return EquipSlot; };

	// For State Checking
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool IsEquipped() const { return bEquipped; };

public:
	void Equipped(FString NewEquipSlot);
	void UnEquipped();

public:
	UPROPERTY(BlueprintAssignable)
	FOnEquipped OnEquipped;
	UPROPERTY(BlueprintAssignable)
	FOnUnEquipped OnUnEquipped;
};
