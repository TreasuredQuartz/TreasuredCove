// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Items/Repository.h"

void FRepository::AddItem(const FItemKey Item)
{
	// If our array contains no items...
	if (Repository.Num() <= 0)
	{
		// ... then just add item.
		Repository.Add(Item);
		return;
	}

	// For each index i; from 0 to last...
	for (int i = 0; i < Repository.Num(); ++i)
	{
		// ... Check if index is our item we are adding to...
		if (Repository[i].Name == Item.Name)
		{
			// ... Finally add the given Item.Quantity to the existing Array Item.Quantity.
			UE_LOG(LogTemp, Warning, TEXT("Item at index: %d  With Name: %s  And Quantity: %d  had  %d Added to it."), i, *(Repository[i].Name.ToString()), Repository[i].Quantity, Item.Quantity);
			Repository[i].Quantity = FMath::Clamp<uint8>(Repository[i].Quantity + Item.Quantity, 0, 255);
			return;
		}
	}

	// Reaching this point in the function means:
	// - Our Category specific array exists
	// - It is not empty
	// - It does not contain the item we are adding to.
	// So just add the item to the array.
	Repository.Add(Item);
	return;
}

void FRepository::SubtractItem(FItemKey Item)
{
	// If our array contains no items...
	if (Repository.Num() <= 0)
	{
		// ... then do nothing.
		return;
	}

	// For each index i; from 0 to last...
	for (int i = 0; i < Repository.Num(); ++i)
	{
		// ... Check if index is our item we are subtracting from...
		if (Repository[i].Name == Item.Name)
		{
			// ... Finally subtract the given Item.Quantity from the existing Array Item.Quantity.
			FItemKey& ItemRef = Repository[i];
			ItemRef.Quantity = FMath::Clamp<uint8>(ItemRef.Quantity - Item.Quantity, 0, 255);
			return;
		}
	}

	// Reaching this point in the function means:
	// - Our Category specific array exists
	// - It is not empty
	// - It does not contain the item we are adding to.
	// So do nothing.
	return;
}

void FRepository::SubtractItem(const uint8 Index, const uint8 Quantity)
{
	// If our array contains no items...
	if (Repository.Num() <= 0)
	{
		// ... then do nothing.
		return;
	}

	Repository[Index].Quantity = FMath::Clamp<uint8>(Repository[Index].Quantity - Quantity, 0, 255);
}

void FRepository::RemoveItem(const uint8 Index)
{
	if (Repository.IsValidIndex(Index))
		Repository.RemoveAt(Index);
}

bool FRepository::GetItemQuantity(const FName Name, int32& Quantity) const
{
	for (int i = 0; i < Repository.Num(); ++i)
	{
		if (Repository[i].Name == Name)
		{
			Quantity = Repository[i].Quantity;
			return true;
		}
	}

	Quantity = -1;
	return false;
}

bool FRepository::GetItemQuantity(const uint8 Index, int32& Quantity) const
{
	if (Repository.IsValidIndex(Index))
	{
		Quantity = Repository[Index].Quantity;
		return true;
	}

	Quantity = -1;
	return false;
}

int32 FRepository::GetItemIndex(const FName Name) const
{
	for (int i = 0; i < Repository.Num(); ++i)
	{
		if (Repository[i].Name == Name)
		{
			return i;
		}
	}

	return -1;
}
