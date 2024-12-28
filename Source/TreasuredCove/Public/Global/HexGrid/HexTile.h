// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/Structs/ProceduralMeshSectionStruct.h"
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

UCLASS(Blueprintable)
class TREASUREDCOVE_API UHexTile : public UDataAsset
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTiles", meta = (ExposeOnSpawn="true", AllowPrivateAccess="true"))
	uint8 Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTiles", meta = (ExposeOnSpawn = "true", AllowPrivateAccess = "true"))
	FVector Location;

	// Set by HexGrid UpdateMesh() function
	int32 Index;
	FLinearColor Color;
	uint8 bHidden:1;
	uint8 bHasConstructed:1;
	uint8 bEnableCollision:1;
	FProceduralMeshSection TileMeshSection;

public:	
	// Sets default values for this actor's properties
	UHexTile();

public:
	void SetTileType(uint8 InTileType);
	void SetTileLocation(const FVector& InLocation);
	void SetTileColor(const FLinearColor& InColor);
	void SetTileIndex(int32 InIndex);
	void SetTileHiddenInGame(bool NewHidden);
	void SetTileMeshSection(FProceduralMeshSection& Section);

	uint8 GetTileType() const;
	FVector GetTileLocation() const;
	FLinearColor GetTileColor() const;
	int32 GetTileIndex() const;
	FProceduralMeshSection& GetTileMeshSection();


	bool IsTileHiddenInGame() const;
};
