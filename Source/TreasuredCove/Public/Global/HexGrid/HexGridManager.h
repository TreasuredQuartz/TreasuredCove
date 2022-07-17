// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAWidget.h"
#include "HexGridManager.generated.h"

class UHexGrid;

UCLASS()
class TREASUREDCOVE_API UHexGridManager : public UGAWidget
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UHexGridManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	uint8 RandomSeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	uint8 RenderRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	uint8 GridWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	uint8 GridHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double VerticalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double HorizontalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double OddHorizontalOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	uint8 TileSize;

	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void BeginPlay();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	UHexGrid* AddHexGrid(const FVector2D& Location);
protected:

public:	

};
