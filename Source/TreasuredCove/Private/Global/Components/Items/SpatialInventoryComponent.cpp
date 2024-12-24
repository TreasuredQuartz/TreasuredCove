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

	for (int32 x = 0; x < GridDimensions.X; ++x)
	{
		for (int32 y = 0; y < GridDimensions.Y; ++y)
		{
			Grid[x][y].Location = FIntPoint(x, y);
		}
	}
}

// Called every frame
//void USpatialInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

// Called to AddItem, will return false if failed.
bool USpatialInventoryComponent::TryAddItem(USpatialInventoryTileItem* Item, const FIntPoint& InLocation)
{
	if (!Item) return false;
	if (!CheckAvailableSpace(Item, InLocation)) return false;

	AddItem(Item, InLocation);

	return true;
}

/** Loops through the shape of the passed in Item
* and Adds the Item's Location as an offset to check 
* if the Item fits in the Grid and does not overlap
* Invalid Items.
*/
bool USpatialInventoryComponent::CheckAvailableSpace(USpatialInventoryTileItem* Item, const FIntPoint& InLocation) const
{
	if (!Item) return false;

	if (Item->GetShape().IsEmpty()) 
	{
		FIntPoint FinalLocation = InLocation;
		if (!IsValidTile(Item, FinalLocation)) return false;
	}
	else 
	{
		for (const FIntPoint& PartialLocation : Item->GetShape())
		{
			FIntPoint FinalLocation = InLocation + PartialLocation;
			if (!IsValidTile(Item, FinalLocation)) return false;
		}
	}
	
	// Every location is inside the grid and does not overlap with invalid items.
	return true;
}

// Returns if the Location is within the Grid
bool USpatialInventoryComponent::IsValidLocation(const FIntPoint& TileLocation) const
{
	return TileLocation.X >= 0
		&& TileLocation.Y >= 0
		&& TileLocation.X < GridDimensions.X
		&& TileLocation.Y < GridDimensions.Y;
}

bool USpatialInventoryComponent::IsValidTile(const USpatialInventoryTileItem* Item, const FIntPoint& TileLocation) const
{
	// Guard against invalid locations
	if (!IsValidLocation(TileLocation)) return false;

	// Guard against already filled tiles
	const FSpatialInventoryTile& OccupiedTile = Grid[TileLocation.X][TileLocation.Y];
	if (OccupiedTile.Item) return false;

	// Search all items if they overlap at the location
	TArray<USpatialInventoryTileItem*> OverlappedItems = GetAllItemsAtLocation(TileLocation);

	// Check each overlapping item if they can co-exist
	for (USpatialInventoryTileItem* InvItem : OverlappedItems)
	{
		// If any item doesn't allow co-existing
		if (InvItem->CanCohabitat(Item, TileLocation) == false)
		{
			return false; // Then we cannot add this item here.
		}
	}

	// Nothing prevents this item from being added here!!!
	return true;
}

TArray<USpatialInventoryTileItem*> USpatialInventoryComponent::GetAllItemsAtLocation(const FIntPoint& TileLocation) const
{
	TArray<USpatialInventoryTileItem*> OverlappingTileItems;

	TMap<FIntPoint, USpatialInventoryTileItem*> Items = GetAllItems();
	TArray<FIntPoint> Keys;
	Items.GetKeys(Keys);
	for (FIntPoint Key : Keys)
	{
		bool bOverlaps = false;
		USpatialInventoryTileItem* InvItem = Items[Key];
		for (FIntPoint LocLocation : InvItem->GetShape())
		{
			if (LocLocation + Key == TileLocation)
			{
				bOverlaps = true;
				break;
			}
		}

		if (bOverlaps)
		{
			OverlappingTileItems.Add(InvItem);
			continue;
		}
	}

	return OverlappingTileItems;
}

TMap<FIntPoint, USpatialInventoryTileItem*> USpatialInventoryComponent::GetAllItems() const
{
	TMap<FIntPoint, USpatialInventoryTileItem*> returnMap;
	returnMap.Empty(GridDimensions.X * GridDimensions.Y);

	/* We have a grid here.
	*	 _________
	*	|_|_|_|_|_|
	*	|_|_|_|_|_|
	*	|_|_|_|_|_|
	* 
	* First, we loop through the columns.
	*	 _________
	*	|X|_|_|_|_|
	*	|X|_|_|_|_|
	*	|X|_|_|_|_|
	*	/\
	*	||
	*	||
	*	The First Column
	*/
	for (int32 x = 0; x < GridDimensions.X; ++x)
	{
		/* Now in the current column:
		*	 _
		*	|_|
		*	|_|
		*	|_|
		* 
		* We loop through each row.
		*	 _
		*	|X| <-- The first row, of the first column, is the first item.
		*	|_|
		*	|_|
		*/
		for (int32 y = 0; y < GridDimensions.Y; ++y) 
		{
			const FSpatialInventoryTile Tile = Grid[x][y];
			USpatialInventoryTileItem* Item = Tile.Item;

			if (Item != nullptr)
			{
				returnMap.Add(FIntPoint(x, y), Item);
			}
		}
	}

	return returnMap;
}

USpatialInventoryTileItem* USpatialInventoryComponent::CreateItem(TSubclassOf<USpatialInventoryTileItem> ItemClass) const
{
	return NewObject<USpatialInventoryTileItem>(GetOwner(), ItemClass);
}

void USpatialInventoryComponent::RemoveItem(USpatialInventoryTileItem* ItemToRemove)
{
	// Guard against null parameters...
	if (!ItemToRemove) return;

	FIntPoint ItemLocation = ItemToRemove->GetLocation();
	FSpatialInventoryTile& Tile = Grid[ItemLocation.X][ItemLocation.Y];
	Tile.Item = nullptr;

	OnInventoryChanged.Broadcast();
}

// The actual addition of the item to the Grid
void USpatialInventoryComponent::AddItem(USpatialInventoryTileItem* Item, const FIntPoint& InLocation)
{
	{ // Scoped variables
		FIntPoint FinalLocation = InLocation;
		FSpatialInventoryTile& Tile = Grid[FinalLocation.X][FinalLocation.Y];
		if (Tile.Item)
		{
			USpatialInventoryTileItem* PrevItem = Tile.Item;
			Tile.Item = Item;
			Item->SetLocation(FinalLocation);
		}
		else if (!Tile.Item)
		{
			Tile.Item = Item;
			Item->SetLocation(FinalLocation);
		}
	}

	OnInventoryChanged.Broadcast();
}



bool USpatialInventoryTileItem::CanCohabitat_Implementation(const USpatialInventoryTileItem* Other, const FIntPoint& InLocation) const
{
	// We can have another item in our space, but not where we are by default
	return InLocation != GetLocation();
}

void USpatialInventoryTileItem::OnCohabitat_Implementation(USpatialInventoryTileItem* Other, const FIntPoint& InLocation)
{
}
