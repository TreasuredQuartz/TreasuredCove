// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayManager.h"
#include "HexGridManager.generated.h"

class AHexGrid;

UCLASS()
class TREASUREDCOVE_API AHexGridManager : public AGameplayManager
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexGridManager();

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	// uint8 RandomSeed;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	// uint8 RenderRange;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	// uint8 GridWidth;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	// uint8 GridHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double VerticalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double HorizontalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double OddHorizontalOffset;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	// uint8 TileSize;

	virtual void OnConstruction(const FTransform& Transform) override;
	// virtual void OnConstruction_DoOnce() override;

	// UFUNCTION(BlueprintCallable, Category = "HexGrid")
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void AddChunk() override;

	// UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	// AHexGrid* AddHexGrid(const FVector2D& Location);
protected:

public:	

};
