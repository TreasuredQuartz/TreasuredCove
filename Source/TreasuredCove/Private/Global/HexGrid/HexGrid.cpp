// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexGrid.h"
#include "HexTile.h"
#include "SimplexNoiseBPLibrary.h"

// Sets default values
UHexGrid::UHexGrid()
{
	TileSize = 100;
	Height = 5;
	Width = 5;
	VerticalOffset = (TileSize * .75);
	HorizontalOffset = (DOUBLE_UE_SQRT_3 * (double)TileSize);
	OddHorizontalOffset = ((DOUBLE_UE_SQRT_3 * (double)TileSize) * .5);
}

#if WITH_EDITOR
void UHexGrid::PostEditChangeProperty(struct FPropertyChangedEvent& e)
{
	if (!e.Property) return;

	if (e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UHexGrid, Height) || 
		e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UHexGrid, Width) || 
		e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UHexGrid, VerticalOffset) || 
		e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UHexGrid, HorizontalOffset) || 
		e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UHexGrid, OddHorizontalOffset))
	{
		bShouldInitializeTiles = true;
	}
	else if (e.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UHexGrid, Noise))
	{
		bShouldInitializeNoise = true;
	}

	Super::PostEditChangeProperty(e);
}
#endif

void UHexGrid::BeginPlay()
{
	// Set properties and initialize grid
	if (Noise.Num() <= 0 || bShouldInitializeNoise)
	{
		Noise = CalcNoise();
		bShouldInitializeNoise = false;
	}
	if (Tiles.Num() <= 0 || bShouldInitializeTiles)
	{
		Tiles.SetNumZeroed(Width);

		uint8 i, x, y;
		for (x = 0; x < Tiles.Num(); ++x)
		{
			Tiles[x].SetNumZeroed(Height);
		}

		i = 0;
		for (y = 0; y < Height; ++y)
		{
			const bool oddRow = y % 2 == 1;
			const float yPos = (Location.Y + y) * VerticalOffset;

			for (x = 0; x < Width; ++x)
			{
				// if (y == (uint8)(Height * 0.5) && x == (uint8)(Width * 0.5)) ;
					
				const float xPos = oddRow ? ((Location.X + x) * HorizontalOffset) + OddHorizontalOffset : (Location.X + x) * HorizontalOffset;
				const FVector2D Pos = FVector2D(xPos, yPos);
				AddTile(Pos, Noise[i]);
				AddTileDelegate.Broadcast(Pos, Noise[i++]);
			}
		}

		bShouldInitializeTiles = false;
	}
}

UHexTile* UHexGrid::AddTile_Implementation(const FVector2D& TileLocation, uint8 TileType)
{
	return nullptr;
}

TArray<uint8> UHexGrid::CalcNoise_Implementation()
{
	uint8 x, y;
	TArray<uint8> Value;

	Value.Reserve(Height * Width);
	for (y = 0; y < Height; ++y)
	for (x = 0; x < Width; ++x)
	{
		Value.Add((uint8)
			(((USimplexNoiseBPLibrary::SimplexNoise2D(x, y) + 1) * 0.5) * 6)
		);
	}

	return Value;
}

