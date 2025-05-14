// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HexTileSelector.generated.h"

class UHexTile;

/**
 *
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UHexTileSelector : public UObject
{
	GENERATED_BODY()

public:
	UHexTileSelector();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "Check Selecting Tile"))
	UHexTile* BP_CheckSelectingTile() const;
	virtual UHexTile* CheckSelectingTile() const PURE_VIRTUAL(UHexTileSelector::CheckSelectingTile, return nullptr;);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex Grid")
	UHexTile* CurrentTile;

	UFUNCTION(BlueprintCallable)
	bool WorldSingleLineTrace(FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel) const;

public:
	UHexTile* TileSelectionCheck() const;
};

