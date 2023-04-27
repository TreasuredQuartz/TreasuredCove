// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

class UInventoryComponent;

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TREASUREDCOVE_API IInventoryInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory", meta = (DisplayName = "GetInventoryComponent"))
	UInventoryComponent* GetInventoryComponent() const;
	virtual UInventoryComponent* GetInventoryComponent_Implementation() const = 0;
};
