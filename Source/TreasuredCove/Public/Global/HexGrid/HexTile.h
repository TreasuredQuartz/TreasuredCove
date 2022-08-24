// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshSectionStruct.h"
#include "HexTile.generated.h"

// class UStaticMeshComponent;

UENUM()
enum class EHexTileType : uint8
{
	INVALID,
	GRASS,
	WATER,
	MAX UMETA(Hidden)
};

UCLASS()
class TREASUREDCOVE_API UHexTile : public UDataAsset
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTiles", meta = (ExposeOnSpawn="true", AllowPrivateAccess="true"))
	uint8 Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTiles", meta = (ExposeOnSpawn = "true", AllowPrivateAccess = "true"))
	FVector Location;

public:	
	// Sets default values for this actor's properties
	UHexTile();

public:
	void SetTileType(uint8 InTileType);
	void SetTileLocation(const FVector& InLocation);

	FVector GetTileLocation() const;
};
