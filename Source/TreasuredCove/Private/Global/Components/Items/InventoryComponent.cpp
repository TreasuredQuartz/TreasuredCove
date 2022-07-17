// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "GAActor.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	bHasInventorySpace = true;
	InventorySize = 5;
	Inventory.Reserve(5);
}

void UInventoryComponent::InitializeComponent()
{
	Inventory.Init(nullptr, InventorySize);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called to check whether the inventory has any space for more items
bool UInventoryComponent::InventoryCheck()
{
	uint8 Len = Inventory.Num();

	for (uint8 i = 0; i < Len; ++i)
	{
		if (Inventory[i] == nullptr)
		{
			bHasInventorySpace = true;
			return true;
		}
	}

	bHasInventorySpace = false;
	return false;
}

// Called to return a location in the inventory that is a nullptr
uint8 UInventoryComponent::FindEmptySlot()
{
	// Find available slot
	for (uint8 i = 0; i < InventorySize; ++i) if (!Inventory[i]) return i;

	return INDEX_NONE;
}

// Called to add item to inventory
bool UInventoryComponent::AddItem(AGAActor* Item)
{
	if (!bHasInventorySpace && !InventoryCheck()) return false;

	Inventory[FindEmptySlot()] = Item;
	return true;
}

// Called to move an item within the inventory
void UInventoryComponent::MoveItem(uint8 From, uint8 To)
{
	AGAActor* LocalItem;

	// Are we moving an object where another object is? 
	if (Inventory[To])
	{
		// We are
		LocalItem = Inventory[To];
		Inventory[To] = Inventory[From];
		Inventory[From] = LocalItem;
	}
	else
	{
		// We are not
		Inventory[To] = Inventory[From];
		Inventory[From] = nullptr;
	}
}

// Called to remove the item from the inventory
void UInventoryComponent::RemoveItem(uint8 Slot)
{
	Inventory[Slot] = nullptr;
}

// Called to return the item at the entered Slot's location
AGAActor* UInventoryComponent::GetItem(uint8 Slot)
{
	return Inventory[Slot];
}

// Called to resize the inventory
void UInventoryComponent::SetInventorySize(uint8 Size)
{
	Inventory.SetNum(Size);
}

