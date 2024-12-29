// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexTile.h"
#include "Global/Libraries/GALibrary.h"
#include "GameplayTileData.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
UHexTile::UHexTile()
{
	bHidden = false;
	bHasConstructed = false;
	bEnableCollision = false;
}

void UHexTile::SetTileType(uint8 InTileType)
{
	Type = InTileType;
}

void UHexTile::SetTileLocation(const FVector& InLocation)
{
	Location = InLocation;
}

void UHexTile::SetTileColor(const FLinearColor& InColor)
{
	Color = InColor;
}

void UHexTile::SetTileIndex(int32 InIndex)
{
	Index = InIndex;
}

void UHexTile::SetTileHiddenInGame(bool NewHidden)
{
	bHidden = NewHidden;
}

void UHexTile::SetTileMeshSection(FProceduralMeshSection& InSection)
{
	TileMeshSection = InSection;
}

uint8 UHexTile::GetTileType() const
{
	return Type;
}


FVector UHexTile::GetTileLocation() const
{
	return Location;
}

FLinearColor UHexTile::GetTileColor() const
{
	return Color;
}

int32 UHexTile::GetTileIndex() const
{
	return Index;
}

FProceduralMeshSection& UHexTile::GetTileMeshSection()
{
	return TileMeshSection;
}

bool UHexTile::IsTileHiddenInGame() const
{
	return bHidden || Type == 0;
}




