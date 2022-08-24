// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/HexGrid/HexTile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
UHexTile::UHexTile()
{

}

void UHexTile::SetTileType(uint8 InTileType)
{
	Type = InTileType;
}

void UHexTile::SetTileLocation(const FVector& InLocation)
{
	Location = InLocation;
}

FVector UHexTile::GetTileLocation() const
{
	return Location;
}




