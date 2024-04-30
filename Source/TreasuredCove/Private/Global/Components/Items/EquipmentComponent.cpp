// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/EquipmentComponent.h"
#include "GAActor.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEquipmentComponent::InitializeEquipmentSlotTransforms(const TArray<FTransform>& Transforms)
{
	EquipmentSlotTransforms.Empty(Transforms.Num());
	for (const FTransform& Transform : Transforms)
	{
		EquipmentSlotTransforms.Add(Transform);
	}
}

int32 UEquipmentComponent::FindSlotWithName(FString& SlotName) const
{
	return EquipmentSlotNames.Find(SlotName);
}

void UEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

AGAActor* UEquipmentComponent::GetItemInNameSlot(FString SlotName)
{
	return UInventoryComponent::GetItem(FindSlotWithName(SlotName));
}

bool UEquipmentComponent::AddItem(AGAActor* Item, FString& SlotName)
{
	if (UEquipmentComponent::AddItem(Item, FindSlotWithName(SlotName)))
	{
		OnItemEquipped.Broadcast(SlotName, Item);
		return true;
	}

	return false;
}

void UEquipmentComponent::RemoveItem(FString& SlotName)
{
	UEquipmentComponent::RemoveItem(FindSlotWithName(SlotName));
}

bool UEquipmentComponent::AddItem(AGAActor* Item, int32 Slot)
{
	if (UInventoryComponent::AddItem(Item, Slot))
	{
		// Item->SetOwner(GetOwner());
		// Item->SetActorRelativeTransform(FTransform::Identity, false, nullptr, ETeleportType::TeleportPhysics);
		Item->SetActorHiddenInGame(false);
		return true;
	}

	return false;
}

void UEquipmentComponent::MoveItem(uint8 From, uint8 To)
{
	UInventoryComponent::MoveItem(From, To);
}

void UEquipmentComponent::RemoveItem(uint8 Slot)
{
	UInventoryComponent::RemoveItem(Slot);
}


