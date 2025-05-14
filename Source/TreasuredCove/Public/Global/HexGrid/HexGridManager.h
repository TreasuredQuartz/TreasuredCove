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
	// uint8 GridWidth;
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	// uint8 GridHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	bool bSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double VerticalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double HorizontalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	double OddHorizontalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	int32 SphereDepth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid")
	FVector RenderAngle;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void AddChunk() override;
	virtual AGameplayVoxel* AddVoxelActor(FVector VoxelSpawnLocation, FIntVector inVoxelIndex, int32 CurrentLOD) override;

protected:

public:	
	UFUNCTION(BlueprintPure, Category = "")
	AHexGrid* GetGridAtIndex(int32 InIndex);
};
