// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/EquipmentComponent.h"
#include "Global/Components/Items/EquippableComponent.h"

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

int32 UEquipmentComponent::FindSlotWithName(const FString& SlotName) const
{
	return EquipmentSlotNames.Find(SlotName);
}

void UEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

AActor* UEquipmentComponent::GetItemInNameSlot(const FString& SlotName)
{
	return UInventoryComponent::GetItem(FindSlotWithName(SlotName));
}

bool UEquipmentComponent::EquipItem(AActor* Item, const FString& SlotName)
{
	if (!Item) return false;
	if (UEquipmentComponent::AddItem(Item, FindSlotWithName(SlotName)))
	{
		if (USkeletalMeshComponent* Mesh = GetOwner()->GetComponentByClass<USkeletalMeshComponent>())
		{
			FAttachmentTransformRules ItemAttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
			Item->AttachToComponent(Mesh, ItemAttachmentRules, Mesh->DoesSocketExist(FName(SlotName)) ? FName(SlotName) : FName());
		}

		Item->SetOwner(GetOwner());
		OnItemEquipped.Broadcast(SlotName, Item);
		return true;
	}

	return false;
}

bool UEquipmentComponent::EquipItemFromInventory(const FString& EquipmentSlotName, UInventoryComponent* InInventory, uint8 InventorySlot)
{
	// Item is being removed from inventory
	AActor* TempItem = InInventory->GetItem(InventorySlot);
	if (!TempItem) {
		// UE_LOG(LogTemp, Warning, TEXT("Tried to equip item that does not exist!"));
		return false;
	}

	// Only remove after we know we can equip
	InInventory->RemoveItem(InventorySlot);

	// Remove our current equipment and store in inventory
	AddEquippedItemToInventory(EquipmentSlotName, InInventory);

	// Finally, Equip the item
	return EquipItem(TempItem, EquipmentSlotName);
}

void UEquipmentComponent::AddEquippedItemToInventory(const FString& EquipmentSlotName, UInventoryComponent* InInventory)
{
	AActor* HeldItem = GetItemInNameSlot(EquipmentSlotName);
	if (HeldItem)
	{
		// HeldItem->OnUnEquipped();
		if (UEquippableComponent* EquipComp = HeldItem->GetComponentByClass<UEquippableComponent>())
		{
			EquipComp->OnUnEquipped.Broadcast();
		}
		
		HeldItem->SetActorHiddenInGame(true);
		InInventory->AddItem(HeldItem);
		RemoveItem(EquipmentSlotName);
	}
}

void UEquipmentComponent::RemoveItem(const FString& SlotName)
{
	UEquipmentComponent::RemoveItem(FindSlotWithName(SlotName));
}

bool UEquipmentComponent::AddItem(AActor* Item, int32 Slot)
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


