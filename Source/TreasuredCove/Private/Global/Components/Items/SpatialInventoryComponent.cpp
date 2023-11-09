// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/SpatialInventoryComponent.h"

// Sets default values for this component's properties
USpatialInventoryComponent::USpatialInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void USpatialInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	TArray<FSpatialInventoryTile> Column;
	Column.Init(FSpatialInventoryTile(), GridDimensions.Y);
	Grid.Init(Column, GridDimensions.X);
}

// Called every frame
//void USpatialInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

// Called to AddItem, will return false if failed.
bool USpatialInventoryComponent::TryAddItem(USpatialInventoryTileItem* Item)
{
	if (Item && !CheckAvailableSpace(Item)) return false;

	AddItem(Item);

	return true;
}

/** Loops through the shape of the passed in Item
* and Adds the Item's Location as an offset to check 
* if the Item fits in the Grid and does not overlap
* Invalid Items.
*/
bool USpatialInventoryComponent::CheckAvailableSpace(USpatialInventoryTileItem* Item) const
{
	for (const FIntVector2& PartialLocation : Item->GetShape())
	{
		FIntVector2 FinalLocation = Item->GetLocation() + PartialLocation;
		if (!IsValidTile(FinalLocation)) return false; // Item is not within Grid

		const FSpatialInventoryTile& OccupiedTile = Grid[FinalLocation.X][FinalLocation.Y];
		if (OccupiedTile.Item && OccupiedTile.Item->CanCohabitat(Item))
			return false; // Item already exists in this space, and cannot share space.
	}

	// Every location is inside the grid and does not overlap with invalid items.
	return true;
}

// Returns if the Location is within the Grid
bool USpatialInventoryComponent::IsValidTile(const FIntVector2& TileLocation) const
{
	return TileLocation.X >= 0
		&& TileLocation.Y >= 0
		&& TileLocation.X < GridDimensions.X
		&& TileLocation.Y < GridDimensions.Y;
}

// The actual addition of the item to the Grid
void USpatialInventoryComponent::AddItem(USpatialInventoryTileItem* Item)
{
	for (const FIntVector2& PartialLocation : Item->GetShape())
	{
		FIntVector2 FinalLocation = Item->GetLocation() + PartialLocation;
		FSpatialInventoryTile& Tile = Grid[FinalLocation.X][FinalLocation.Y];
		if (Tile.Item && Tile.Item->CanCohabitat(Item))
		{
			USpatialInventoryTileItem* PrevItem = Tile.Item;
			Tile.Item = Item;
			PrevItem->OnCohabitat(Item);
		}
	}
}



bool USpatialInventoryTileItem::CanCohabitat_Implementation(USpatialInventoryTileItem* Other) const
{
	return false;
}

void USpatialInventoryTileItem::OnCohabitat_Implementation(USpatialInventoryTileItem* Other)
{
	Items.Add(Other);
}
