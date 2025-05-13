// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HexGridEditor.generated.h"

class AHexGrid;
class AHexGridManager;
class AHexGridTown;
class UHexTile;
class UHexTileSelector;

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UHexGridEditor : public UObject,
	public FTickableGameObject
{
	GENERATED_BODY()

	/* Begin Private Properties */
private:
	// The state of whether this hex grid is actively being edited.
	uint8 bIsEditing : 1;
	// Whether this editor has been initialized or not.
	uint8 bInitialized : 1;
	// Whether BeginPlay has been called for this object or not.
	uint8 bHasBegunPlay : 1;

	/* End Private Properties */
public:
	// Set default properties in constructor
	UHexGridEditor();

	// Called when play has been detected in the parent UWorld.
	void BeginPlay();

	// Called before editing begins; which may happen before BeginPlay has been called.
	void Initialize();

	/* Begin FTickable Game Object Overrides */
protected:
	// Called every frame
	void Tick(float DeltaTime) override;
	// Determines whether this object should Tick() every frame.
	bool IsTickable() const override;
	// Determines whether this object should Tick() while in editor.
	bool IsTickableInEditor() const override;
	// Determines whether this object should Tick() while in paused in-game.
	bool IsTickableWhenPaused() const override;
	// Bruh idk.
	TStatId GetStatId() const override;

	/* End FTickable Game Object Overrides */


	/* Begin Public Properties */
public:
	// The Selector class for determing the interaction method for editing the Hex Grid.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Grid")
	TSubclassOf<UHexTileSelector> TileSelectorClass;

	AHexGridManager* HexManager;
	UHexTileSelector* TileSelector;

	/* End Public Properties */

	/* Begin Public Methods */
public:
	UFUNCTION(BlueprintCallable, Category = "Hex Grid")
	void StartEditing();
	UFUNCTION(BlueprintCallable, Category = "Hex Grid")
	void StopEditing();
	UFUNCTION(BlueprintCallable, Category = "Hex Grid")
	bool IsEditing() const { return bIsEditing; };

	/* End Public Methods */
};
