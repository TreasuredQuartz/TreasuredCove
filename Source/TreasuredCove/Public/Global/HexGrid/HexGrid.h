// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayVoxel.h"
#include "Global/Structs/ProceduralMeshSectionStruct.h"
#include "HexGrid.generated.h"

class UHexTile;
class URealtimeMeshComponent;
class URealtimeMeshSimple;

USTRUCT(BlueprintType)
struct FHexCoord { // Vector storage, cube constructor
	GENERATED_BODY()

private:
	FIntVector3 Vector;

public:
	FHexCoord() : Vector(FIntVector3()) {}
	FHexCoord(int q, int r, int s) : Vector{ q, r, s } {
		check(q + r + s == 0);
	};

	inline int q() { return Vector[0]; }
	inline int r() { return Vector[1]; }
	inline int s() { return Vector[2]; }

	// Compares
	FORCEINLINE bool operator == (const FHexCoord& OtherInfo) const
	{
		return this->Vector == OtherInfo.Vector;
	}
	// Compares
	FORCEINLINE bool operator != (const FHexCoord& OtherInfo) const
	{
		return this->Vector != OtherInfo.Vector;
	}
};


// Hash function for TMap Compatibility
#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FHexCoord& Vector);
#else // optimize by inlining in shipping and development builds
FORCEINLINE uint32 GetTypeHash(const FHexCoord& Vector)
{
	uint32 Hash = FCrc::MemCrc32(&Vector, sizeof(Vector));
	return Hash;
}
#endif


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
	bool bSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = "true"))
	float TileOffset;
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
	TMap<FHexCoord, UHexTile*> Tiles;
	TArray<FVector> TileLocations;

public:	
	// Sets default values for this actor's properties
	AHexGrid();
	virtual ~AHexGrid() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	uint8 bFlatTop : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	FVector NormalVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HexGrid", meta = (ExposeOnSpawn = "true"))
	FVector2D Location;
	UPROPERTY(EditDefaultsOnly, Category = "HexGrid")
	TArray<UMaterialInterface*> Materials;
	UPROPERTY(EditDefaultsOnly, Category = "HexGrid")
	UMaterialInterface* RTMeshMaterial;
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
	UHexTile* GetTileFromIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	const FHexCoord& GetTileCoordFromIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	FTransform GetTileCollisionTransform(int32 CollisionInstanceIndex) const;

	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void SetTileVertexColor(const FLinearColor& Color, const FHexCoord& InHexCoordinate);

	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void SetTileHiddenInGame(bool NewHidden, const FHexCoord& InHexCoordinate);

	UFUNCTION(BlueprintCallable, Category = "HexGrid")
	void SetSphere(bool bNewSphere) { bSphere = bNewSphere; };
};
