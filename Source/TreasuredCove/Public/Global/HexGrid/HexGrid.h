// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayVoxel.h"
#include "ProceduralMeshSectionStruct.h"
#include "HexGrid.generated.h"

class UHexTile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAddTileDelegate, const FVector2D&, TileLocation, uint8, TileType);

UCLASS()
class TREASUREDCOVE_API AHexGrid : public AGameplayVoxel
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess, ExposeOnSpawn = "true"))
	uint8 SurfaceHeight;

	UPROPERTY()
	URuntimeMeshComponentStatic* Mesh;
protected:
	TArray<uint8> Types;
	TArray<int32> Height;
	TArray<UHexTile*> Tiles;
	uint8 RandomSeed;

public:	
	// Sets default values for this actor's properties
	AHexGrid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	FVector2D Location;

	UPROPERTY(EditDefaultsOnly, Category = "HexGrid")
	class UGameplayTileData* HexTileData;
	UPROPERTY(EditDefaultsOnly, Category = "HexGrid")
	TArray<UMaterialInterface*> Materials;
protected:

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
#endif

	virtual void BeginPlay() override;
	virtual void Initialize(int32 inRandomSeed, int32 inVoxelSize, int32 inChunkLineElements, FIntVector inChunkIndex) override;

	UPROPERTY(BlueprintAssignable, Category = "HexGrid")
	FAddTileDelegate AddTileDelegate;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	UHexTile* AddTile(const FVector& TileLocation, uint8 TileType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	TArray<uint8> CalcTypes() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	TArray<int32> CalcHeight() const;

	void InitializeNoise();
	void InitializeTiles();
	virtual void BeforeChunkGenerated() override;
	void GenerateHexGrid();
	virtual void ChunkGenerating(const FIntVector& CurrentLocation, int32 Index) override;
	virtual void AfterChunkGenerated() override;

	void UpdateMesh();
};
