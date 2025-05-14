// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/HexGrid/HexTileSelector.h"
#include "Kismet/KismetMathLibrary.h"


UHexTileSelector::UHexTileSelector()
{
}

UHexTile* UHexTileSelector::TileSelectionCheck() const
{
	if (UHexTile* Tile = BP_CheckSelectingTile())
	{
		return Tile;
	}

	return CheckSelectingTile();
}

bool UHexTileSelector::WorldSingleLineTrace(FHitResult& OutHit, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel) const
{
	if (!GetWorld()) return false;

	return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel);
}

