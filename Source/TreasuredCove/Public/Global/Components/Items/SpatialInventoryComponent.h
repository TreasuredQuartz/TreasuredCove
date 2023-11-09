// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpatialInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(Blueprintable)
class TREASUREDCOVE_API USpatialInventoryTileItem : public UObject
{
	GENERATED_BODY()

private:
	FIntVector2 Location;
	TArray<FIntVector2> Shape;
	UObject* Object;

protected:
	TArray<USpatialInventoryTileItem*> Items;

public:
	// Returns Location in Inventory
	FIntVector2 GetLocation() const { return Location; };

	void SetLocation(FIntVector2& NewLocation) { Location = NewLocation; };

	// Returns the space this item takes up in inventory
	TArray<FIntVector2> GetShape() const { return Shape; };

	void SetShape(TArray<FIntVector2>& NewShape) { Shape = NewShape; };

	// Returns if other items can exist in the same space as this item
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanCohabitat(USpatialInventoryTileItem* Other) const;

	// Event for when an item is added to the same space as this item
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnCohabitat(USpatialInventoryTileItem* Other);
};

USTRUCT()
struct TREASUREDCOVE_API FSpatialInventoryTile
{
	GENERATED_BODY()

public:
	FIntVector2 Location;
	USpatialInventoryTileItem* Item;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API USpatialInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpatialInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	//
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FIntVector2 GridDimensions;

	//
	TArray<TArray<FSpatialInventoryTile>> Grid;

public:	
	// Called every frame /* Disabled */
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Checks if the item can fit in the grid, then adds the item if possible
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryAddItem(USpatialInventoryTileItem* Item);

	// Checks that the space is valid and does not overlap filled tiles
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CheckAvailableSpace(USpatialInventoryTileItem* Item) const;

	// Checks that the Location is inside of the grid space
	bool IsValidTile(const FIntVector2& TileLocation) const;

private:
	// Adds Item without checks, called via TryAddItem()
	void AddItem(USpatialInventoryTileItem* Item);
};
