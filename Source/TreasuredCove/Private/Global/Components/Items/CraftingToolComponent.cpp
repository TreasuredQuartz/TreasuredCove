// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Components/Items/CraftingToolComponent.h"
#include "Global/Components/Items/CraftingComponent.h"

// Sets default values for this component's properties
UCraftingToolComponent::UCraftingToolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCraftingToolComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCraftingToolComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UCraftingToolComponent::TryCraftItem(TArray<UCraftingComponent*>& InItems, UObject* OutItem)
{
	for (int i = 1; i < InItems.Num(); ++i)
	{
		const auto& PrevItem = InItems[i - 1];
		const auto& CurItem = InItems[i];

		if (!PrevItem->CraftableItems.Contains(CurItem)) return false;
	}

	CraftItem(InItems);
	return true;
}

//TArray<FMaterial> Materials;
//TMap<Key, Value> ElementCount;
//
//for (const auto& Material : Materials)
//{
//	key = Key(Material.Name);
//	if (ElementCount.Contains(key))
//	{
//		ElementCount.Find(key) += 1;
//	}
//	else
//	{
//		ElementCount.Add(key, 1);
//	}
//}
// 
//FMaterial BaseMaterial;
//BaseMaterial = ElementCount.MaxCount();
// 
//for (const auto& Material : Materials)
//{
//	if (BaseMaterial == Material)
//	{
//		BaseMaterial.Combine(Material);									// This would combine stats and keep the same base material
//	}
//	else if (BaseMaterial.ListOfMorphableMaterials.Contains(Material))
//	{
//		BaseMaterial.Morph(Material);									// This would combine stats and change the base material
//	}
//	else
//	{
//		BaseMaterial.AddImpurity(Material);								// this would give specific stat combinations and give the base material a percentage of different materials (impure)
//	}
//}
//bool CraftItem(OutNewItem, InItemStencil, InTArray<FMaterial> InMaterials, InItemCraftingTool)
//{
//	OutNewItem = InItemStencil.TryCrafting(InMaterials, InItemCraftingTool));
//	return (NewItem != nullptr);
//}
// 
//bool AttuneMagicalItem(UnattunedItem, AttunmentType)
//{
//	if (UnattunedItem.CanAttuneItem(AttunmentType))
//	{
//		UnattunedItem.AttuneItem(AttachmentType);
//	}
//}
// 
//bool CombineItems(OutNewItem, InItem1, InItem2, InItemCraftingTool)
//{
//	if (InItemCraftingTool.CanCombineItems(InItem1, InItem2))
//	{
//		OutNewItem = InItemCraftingTool.CombineItems(InItem1, InItem2);
//		return true;
//	}
// 
//	return false;
//}
//for (const auto& Recipe : InRecipe)
//{
//	if (Recipe.Name == FName("Metal"))
//	{
//	}
//}
void UCraftingToolComponent::CraftItem(TArray<UCraftingComponent*>& InItems)
{
	for (const auto& Item : InItems)
	{

	}
}

