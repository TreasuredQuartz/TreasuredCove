// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayVoxel.h"
#include "ProceduralMeshSectionStruct.h"
#include "HexGrid.generated.h"

class UHexTile;
class URealtimeMeshComponent;
class URealtimeMeshSimple;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAddTileDelegate, const FVector2D&, TileLocation, uint8, TileType);

UCLASS()
class TREASUREDCOVE_API AHexGrid : public AGameplayVoxel
{
	GENERATED_BODY()
private:
	bool bGeneratedMeshRebuildPending = false;
	bool bResetOnRebuild = true;
	bool bShouldInitializeNoise = true;
	bool bShouldInitializeTiles = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	float VerticalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	float HorizontalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	float OddHorizontalOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	uint8 SurfaceHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	FVector Scale3D;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	URealtimeMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	URealtimeMeshSimple* RTMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UInstancedStaticMeshComponent* MeshCollisionInstances;

	int32 ChunkLineElementsP3;
protected:
	uint8 RandomSeed;
	TArray<uint8> Types;
	TArray<int32> Height;
	TArray<UHexTile*> Tiles;
	TArray<FVector> TileLocations;

public:	
	// Sets default values for this actor's properties
	AHexGrid();
	virtual ~AHexGrid() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	FVector NormalVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	FVector2D Location;
	UPROPERTY(EditDefaultsOnly, Category = "HexGrid")
	TArray<UMaterialInterface*> Materials;
	UPROPERTY(EditDefaultsOnly, Category = "HexGrid")
	class UGameplayTileData* MeshSectionData;
public:
	//~ Begin UObject/AActor Interface
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;
	virtual void Destroyed() override;

	// these are called when Actor exists in a sublevel that is hidden/shown
	virtual void PreRegisterAllComponents() override;
	virtual void PostUnregisterAllComponents() override;

#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif

protected:

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
#endif

	void ExecuteRebuildGeneratedMeshIfPending();

	// Allows us to draw in the blueprint editor
	virtual bool ShouldTickIfViewportsOnly() const override
	{
		return true;
	}
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;
	virtual void Initialize(int32 inRandomSeed, int32 inVoxelSize, int32 inChunkLineElements, FIntVector inChunkIndex) override;

	UPROPERTY(BlueprintAssignable, Category = "HexGrid")
	FAddTileDelegate AddTileDelegate;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	UHexTile* AddTile(const FVector& TileLocation, int32 CurrentTileNum);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	TArray<uint8> CalcTypes() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HexGrid")
	TArray<int32> CalcHeight() const;

	void InitializeNoise();
	void InitializeTiles();
	virtual void BeforeChunkGenerated() override;
	void GenerateHexSphere();
	void GenerateHexGrid();
	virtual void ChunkGenerating(const FIntVector& CurrentLocation, int32 Index) override;
	virtual void AfterChunkGenerated() override;

	void OnGenerateMesh();
	void ConstructMesh();
	void UpdateMesh();
	void UpdateMeshSection(int32 MeshSectionIndex);

public:
	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	FTransform GetTileTransform(int32 TileIndex) const;

	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void SetTileVertexColor(const FLinearColor& Color, int32 TileIndex);

	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void SetTileHiddenInGame(bool NewHidden, int32 TileIndex);
};
