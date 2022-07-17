// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/ItemKeyStruct.h"
#include "Repository.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FRepository
{
	GENERATED_BODY()
	
public:
	// Actual container for items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items|Repository")
	TArray<FItemKey> Repository;

	// Called to add to item in Repository
	// - Only pass positive numbers. 
	// - (Use SubtractItem if you wish to subtract instead)
	void AddItem(const FItemKey Item);

	// Called to subtract from item in repository
	// - Only pass positive numbers. 
	void SubtractItem(FItemKey Item);
	void SubtractItem(const uint8 Index, const uint8 Quantity);

	// Called when Quantity reaches zero.
	// - Removes item from array
	// - Removes Category if array is empty
	void RemoveItem(const uint8 Index);

	// Called to find specific item.Quantity in category.
	// Returns false if unable.
	bool GetItemQuantity(const FName Name, int32& Quantity) const;
	// Called to find specific item.Quantity in category.
	// - Overloaded for index version
	// Returns false if unable.
	bool GetItemQuantity(const uint8 Index, int32& Quantity) const;

	// Called to find Item's index in array
	int32 GetItemIndex(const FName Name) const;

	// Returns the size of the inner array
	inline uint32 GetRepositorySize() { return Repository.Num(); };
};
