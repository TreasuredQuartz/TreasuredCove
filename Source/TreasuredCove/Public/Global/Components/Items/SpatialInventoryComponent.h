// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpatialInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpatialInventoryChanged); // Delegate declared in parent

UCLASS(Blueprintable)
class TREASUREDCOVE_API USpatialInventoryTileItem : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName Name;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FIntPoint Location;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<FIntPoint> Shape;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UTexture2D* Icon;

public:
	// Returns Location in Inventory
	FIntPoint GetLocation() const { return Location; };

	void SetLocation(FIntPoint NewLocation) { Location = NewLocation; };

	// Returns the space this item takes up in inventory
	TArray<FIntPoint> GetShape() const { return Shape; };

	void SetShape(TArray<FIntPoint>& NewShape) { Shape = NewShape; };

	// Returns if other items can exist in the same space as this item
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanCohabitat(const USpatialInventoryTileItem* Other, const FIntPoint& InLocation) const;

	// Event for when an item is added to the same space as this item
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnCohabitat(USpatialInventoryTileItem* Other, const FIntPoint& InLocation);
};

USTRUCT()
struct TREASUREDCOVE_API FSpatialInventoryTile
{
	GENERATED_BODY()

public:
	FIntPoint Location;
	USpatialInventoryTileItem* Item;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API USpatialInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpatialInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Items|Inventory")
	FOnSpatialInventoryChanged OnInventoryChanged;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	//
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D GridDimensions;

	//
	TArray<TArray<FSpatialInventoryTile>> Grid;

public:	
	// Called every frame /* Disabled */
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Checks if the item can fit in the grid, then adds the item if possible
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryAddItem(USpatialInventoryTileItem* Item, const FIntPoint& InLocation);

	// Checks that the space is valid and does not overlap filled tiles
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CheckAvailableSpace(USpatialInventoryTileItem* Item, const FIntPoint& InLocation) const;

	// Checks that the Location is inside of the grid space
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsValidLocation(const FIntPoint& TileLocation) const;

	// Checks that the Location is valid and either unoccupied or both can occupy the space.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsValidTile(const USpatialInventoryTileItem* Item, const FIntPoint& TileLocation) const;

	// Returns all items that overlap at this location.
	TArray<USpatialInventoryTileItem*> GetAllItemsAtLocation(const FIntPoint& TileLocation) const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Inventory")
	void GetGridDimensions(int32& X, int32& Y) {
		X = (int32)GridDimensions.X;
		Y = (int32)GridDimensions.Y; 
	};

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Inventory")
	void IndexToTile(int32 index, int32& TileX, int32& TileY) { 
		TileX = index % (int32)GridDimensions.X;
		TileY = index / (int32)GridDimensions.X;
	};

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Inventory")
	TMap<FIntPoint, USpatialInventoryTileItem*> GetAllItems() const;

	/** Returns a Spatial Inventory Tile Item.
	* This should be called in UI events while trying to add new items to the inventory.
	* Widgets are Client only, Components exist on the server.
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (DeterminesOutputType = "ItemClass"))
	USpatialInventoryTileItem* CreateItem(TSubclassOf<USpatialInventoryTileItem> ItemClass) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItem(USpatialInventoryTileItem* ItemToRemove);

private:
	// Adds Item without checks, called via TryAddItem()
	void AddItem(USpatialInventoryTileItem* Item, const FIntPoint& Location);
};
