// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexGridManager.h"

// Sets default values
UHexGridManager::UHexGridManager()
{
	RenderRange = 2;
	GridWidth = 4;
	GridHeight = 2;
	VerticalOffset = (TileSize * .75);
	HorizontalOffset = (DOUBLE_UE_SQRT_3 * (double)TileSize);
	OddHorizontalOffset = ((DOUBLE_UE_SQRT_3 * (double)TileSize) * .5);
}

void UHexGridManager::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("Begin play for hex grid manager..."));
	int8 x, y;
	const int32 xHexGridSize = (HorizontalOffset) * (GridWidth / 2);
	const int32 yHexGridSize = (VerticalOffset) * (GridHeight / 1.33);

	for (y = -RenderRange; y <= RenderRange; ++y)
	{
		const int32 yPos = y * yHexGridSize;
		for (x = -RenderRange; x <= RenderRange; ++x)
		{
			const int32 xPos = x * xHexGridSize;
			const FVector2D Pos = FVector2D(xPos, yPos);

			AddHexGrid(Pos);
		}
	}
}

UHexGrid* UHexGridManager::AddHexGrid_Implementation(const FVector2D& Location)
{
	return nullptr;
}
