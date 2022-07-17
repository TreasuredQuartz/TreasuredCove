// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MCWorld_Voxel.h"
#include "ProceduralMiniMap.generated.h"

class UProceduralMeshComponent;

struct FProceduralMeshBuffer
{
private:
	FProceduralMeshSection Section;

public:
	void AppendQuad(const FVector& topLeft, const FVector& topRight, const FVector& bottomLeft, const FVector& bottomRight, const FVector& quadNormal)
	{
		// 2, 1, 0, 0, 3, 2
		const FVector LocVoxelNormals[4] = { quadNormal };
		const FVector2D LocVoxelUVs[] = { FVector2D(0.00000, 0.00000), FVector2D(0.00000, 1.00000), FVector2D(1.00000, 1.00000), FVector2D(1.00000, 0.00000) };
		const FColor LocVoxelColors[] = { FColor(), FColor(), FColor(), FColor() };

		Section.Triangles.Add(2 + Section.NumTriangles);
		Section.Triangles.Add(1 + Section.NumTriangles);
		Section.Triangles.Add(0 + Section.NumTriangles);

		Section.Triangles.Add(0 + Section.NumTriangles);
		Section.Triangles.Add(3 + Section.NumTriangles);
		Section.Triangles.Add(2 + Section.NumTriangles);

		Section.NumTriangles += 4;

		Section.Vertices.Add(topLeft);
		Section.Vertices.Add(topRight);
		Section.Vertices.Add(bottomLeft);
		Section.Vertices.Add(bottomRight);

		Section.Normals.Append(LocVoxelNormals, UE_ARRAY_COUNT(LocVoxelNormals));
		Section.UVs.Append(LocVoxelUVs, UE_ARRAY_COUNT(LocVoxelUVs));
		Section.VertexColors.Append(LocVoxelColors, UE_ARRAY_COUNT(LocVoxelColors));
	}

	const TArray<FVector>& GetVertices()
	{
		return Section.Vertices;
	}

	const TArray<int32>& GetTriangles()
	{
		return Section.Triangles;
	}

	const TArray<FVector>& GetNormals()
	{
		return Section.Normals;
	}

	const TArray<FVector2D>& GetUVs()
	{
		return Section.UVs;
	}

	const TArray<FProcMeshTangent>& GetTangents()
	{
		return Section.Tangents;
	}

	const TArray<FColor>& GetColors()
	{
		return Section.VertexColors;
	}

	bool GetCollision()
	{
		return Section.bEnableCollision;
	}

	uint32 GetMaterialIndex()
	{
		return Section.MaterialIndex;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UProceduralMiniMap : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProceduralMiniMap();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FProceduralMeshBuffer VertexBuffer;
	FVector MiniMapLocation;

	int32 MiniMapElements;
	int32 MiniMapElementsHalf;

	float MiniMapElementSize;
	float MiniMapElementSizeHalf;

	int32 RayCastRange;

	// 
	// Positive Values add 0s behind MiniMapElementSize
	// Negative Values add 0s in front of MiniMapElementSize
	int8 MiniMapScale;

	int32 MiniMapSize;
	int32 MiniMapSizeHalf;

	TArray<bool> Faces;
	TArray<FVector> TestDirections;

	UPROPERTY()
	UProceduralMeshComponent* PMesh;
	UPROPERTY()
	TArray<UMaterialInterface*> Materials;

	UFUNCTION()
	void UpdateLocation(FVector NewLocation);

	float ApplyScale(int32 Size);
	bool CastRay(FHitResult& Hit, const FVector& Start, const FVector& Direction);
		
	void UpdateMesh();
};
