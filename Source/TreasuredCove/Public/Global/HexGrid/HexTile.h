// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAWidget.h"
#include "HexTile.generated.h"

class UStaticMeshComponent;

UENUM()
enum class EHexTileType : uint8
{
	INVALID,
	GRASS,
	WATER,
	MAX UMETA(Hidden)
};

UCLASS()
class TREASUREDCOVE_API UHexTile : public UGAWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexTiles", meta = (ExposeOnSpawn="true"))
	uint8 TileType;

public:	
	// Sets default values for this actor's properties
	UHexTile();

};
