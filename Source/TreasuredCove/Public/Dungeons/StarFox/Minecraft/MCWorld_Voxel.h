// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayVoxel.h"

#include "ProceduralMeshComponent.h"
#include "DTMCBlockInfoStruct.h"
#include "MCWorld_Voxel.generated.h"

class UInstancedStaticMeshComponent;

struct FProceduralMeshSection
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;
	UMaterialInterface* Material;

	bool bEnableCollision = false;
	uint32 ElementID = 0;
	uint32 NumTriangles = 0;
	uint32 MaterialIndex = 0;
};

struct FMCWorld_Building
{

public:
	TMap<FIntVector, int32> Blocks;

	FMCWorld_Building()
	{
	}

	FMCWorld_Building(TMap<FIntVector, int32> NewBlocks)
	{
		for (auto& Elem : NewBlocks)
		{
			Blocks.Add(Elem.Key, Elem.Value);
		}
	}
};

UCLASS()
class TREASUREDCOVE_API AMCWorld_Voxel : 
	public AGameplayVoxel
{
	GENERATED_BODY()

#pragma region PrivateVariables
private:
	FVector UpVector;
	TArray<FIntVector> TreeCenters;
	TArray<FIntVector> TownCenters;
	TArray<FIntVector> IronOrePocketCenters;
	TArray<FIntVector> DiamondOrePocketCenters;
	TArray<int32> Height;
	TArray<int32> Moisture;
	TArray<int32> Temperature;
	TArray<int32> TreeType;
	FRandomStream RandomStream;
	TArray< FMCWorld_Building > HouseStructures;

	bool bConstructionDoOnce = false;


#if WITH_EDITOR
	bool bSelectedInEditor = false;
#endif
#pragma endregion

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSpawnItems = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableCollision;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	TArray<FMCWorld_ChunkFieldData> BlockData;*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	UDataTable* BlockDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	TArray<class UInstancedStaticMeshComponent*> CustomMeshInstances;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInterface*> Materials;*/


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float XMult = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YMult = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ZMult = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Freq = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SurfaceHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SeaLevelHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector SelectedLocation;
	UMaterialInterface* SpawnVoxelMaterial;

	UPROPERTY()
	TArray< int32 > ChunkFields;
	UPROPERTY()
	UProceduralMeshComponent* PMesh;

	UFUNCTION(BlueprintNativeEvent)
	TArray<int32> CalcNoise();
	UFUNCTION(BlueprintNativeEvent)
	void AddVoxelInstance(FVector Location, int32 InstanceType);
public:	
	// Sets default values for this actor's properties
	AMCWorld_Voxel();

	// AMCWorld_Manager* Manager;
protected:
	// Begin AActor Interface
	// Called when the game starts or when spawned
	// virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
	virtual void PostInitProperties() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void OnConstruction_DoOnce();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;
	// End AActor Interface

	void OnObjectSelected(UObject* Object);
	void OnSelected_Editor();
	void OnUnSelected_Editor();
	UFUNCTION(BlueprintCallable, Category = "Minecraft")
	void SetSelectedVoxel(int32 Value);
#endif

	virtual void Initialize(int32 inRandomSeed, int32 inVoxelSize, int32 inChunkLineElements, FIntVector inChunkIndex) override;
	void SetVoxel(FVector Location, int32 Value);
	void VoxelInteract(FVector Location, AActor* InteractingActor);

private:
	// Begin AGameplayVoxel Interface //
	virtual void BeforeChunkGenerated() override;
	virtual void ChunkGenerating(FIntVector CurrentLocation, int32 Index) override;
	virtual void AfterChunkGenerated() override;
	// End AGameplayVoxel Interface //

	// Initialization
	void InitBuildings();
	void InitSmallHouse();
	void InitLargeHouse();
	void InitBlacksmith();
	void InitTower();

	// Generation
	void GenerateGround(FIntVector Location, int32 Index, int32 Height, int32 Biome);
	void GenerateTrees();
	void GenerateTowns();
	void GenerateOrePockets();
	void GenerateSmallHouse(FIntVector TownCenter);
	void GenerateBlacksmith(FIntVector TownCenter);
	void GenerateTemple(FIntVector TownCenter);
	void GenerateTower(FIntVector TowerCenter);

	// Updating
	void UpdateChunkFromData();
	void UpdateMesh();
	void UpdateExtras();

	// Convenience
	void CreateCubeMesh(FProceduralMeshSection& MeshSection, const int32& InIndex, const int32& x, const int32& y, const int32& z, int32& Triangle_Num);
	void CreateStairMesh(FProceduralMeshSection& MeshSection, const int32& InIndex, const int32& x, const int32& y, const int32& z, int32& Triangle_Num);
	void CreateSlabMesh(FProceduralMeshSection& MeshSection, const int32& InIndex, const int32& x, const int32& y, const int32& z, int32& Triangle_Num);
	void CreatePostMesh(FProceduralMeshSection& MeshSection, const int32& InIndex, const int32& x, const int32& y, const int32& z, int32& Triangle_Num);

	bool InRange(int32 Value, int32 Range);
};
