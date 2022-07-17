// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ProceduralMeshComponent.h"
#include "ProceduralBuilding.generated.h"

struct FProceduralMeshSection
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;
	UMaterialInterface* Material;

	uint32 ElementID = 0;
};

UCLASS()
class PROCEDURALBUILDING_API AProceduralBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralBuilding();

protected:
	// Called in editor and on begin play
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Our Building Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "false"))
	UProceduralMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building | Material")
	UMaterialInterface* MasterMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building | Texture")
	UTexture2D* TextureAtlas_Diffuse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building | Texture")
	UTexture2D* TextureAtlas_Mixture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building | Texture")
	UTexture2D* TextureAtlas_Normal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building | Texture")
	int32 TileSize = 32;


	// Update Mesh
	void UpdateMesh(const FTransform& Transform);
	void GetVerticesFromStaticMesh(TArray<FVector> &OutVertices, FTransform Tranform, UStaticMesh* InMesh);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMeshComponent*> BuildingStructures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building | Structure")
	int32 NumFloors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building | Structure")
	int32 NumWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building | Structure")
	int32 NumDepth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	UStaticMesh* DefaultWallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	UStaticMesh* FirstFloorWallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	UStaticMesh* RoofWallMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	UStaticMesh* DoorMesh;

	int32 DoorPosition;
	int WallSize;
	int WallSizeHalf;
	int WallSizeQuarter;
	int32 ZTile;
	int32 YTile;
	int32 XTile;
};
