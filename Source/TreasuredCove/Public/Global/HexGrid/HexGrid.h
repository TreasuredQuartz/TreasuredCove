// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAWidget.h"
#include "HexGrid.generated.h"

class UHexTile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAddTileDelegate, const FVector2D&, TileLocation, uint8, TileType);

UCLASS()
class TREASUREDCOVE_API UHexGrid : public UGAWidget
{
	GENERATED_BODY()
private:
	bool bShouldInitializeNoise = true;
	bool bShouldInitializeTiles = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess, ExposeOnSpawn = "true"))
	float VerticalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess, ExposeOnSpawn = "true"))
	float HorizontalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess, ExposeOnSpawn = "true"))
	float OddHorizontalOffset;

protected:
	TArray<uint8> Noise;
	TArray<TArray<UHexTile*>> Tiles;
	uint8 RandomSeed;

public:	
	// Sets default values for this actor's properties
	UHexGrid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	FVector2D Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	uint8 Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	uint8 Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	uint8 TileSize;
protected:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
#endif

	// Called manually by children
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void BeginPlay();

	UPROPERTY(BlueprintAssignable, Category = "HexGrid")
	FAddTileDelegate AddTileDelegate;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	UHexTile* AddTile(const FVector2D& TileLocation, uint8 TileType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	TArray<uint8> CalcNoise();
};
