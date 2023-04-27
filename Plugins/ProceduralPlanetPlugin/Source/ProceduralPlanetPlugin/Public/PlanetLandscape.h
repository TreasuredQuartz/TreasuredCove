// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuntimeMeshCore.h"
#include "UObject/NoExportTypes.h"

#include "PlanetMeshSettings.h"
#include "PlanetShapeGenerator.h"
#include "PlanetLandscape.generated.h"

// 100,000
// constexpr int PlanetSize = 100000;

class URuntimeMeshComponentStatic;
class UPlanetQuadTree;

struct FPlanetMeshSection
{
public:
	FPlanetMeshSection() :
		Material(nullptr),
		Vertices(TArray<FVector>()),
		Triangles(TArray<int32>()),
		Normals(TArray<FVector>()),
		UVs(TArray<FVector2D>()),
		Tangents(TArray<struct FRuntimeMeshTangent>()),
		VertexColors(TArray<FColor>())
	{}

public:
	uint32 ElementID = 0;
	UMaterialInterface* Material;
	ERuntimeMeshUpdateFrequency UpdateFrequency = ERuntimeMeshUpdateFrequency::Frequent;
	bool bEnableCollision = false;

public:
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<struct FRuntimeMeshTangent> Tangents;
	TArray<FColor> VertexColors;
	uint32 NumTriangles = 0;

public:
	void ClearAllData()
	{
		ElementID = 0;
		bEnableCollision = false;
		Vertices.Empty();
		Triangles.Empty();
		Normals.Empty();
		UVs.Empty();
		Tangents.Empty();
		VertexColors.Empty();
		NumTriangles = 0;
	};
};

/**
 * 
 */
UCLASS()
class PROCEDURALPLANETPLUGIN_API UPlanetLandscape : public UObject
{
	GENERATED_BODY()
public:
	FPlanetShapeGenerator ShapeGenerator;
	FPlanetMeshSettings MeshSettings;

public:
	UPlanetQuadTree* QuadTree;
	URuntimeMeshComponentStatic* RuntimeMesh;

	FVector PlanetLocation;

	FVector localUp;
	FVector axisA;
	FVector axisB;

	bool bInitialized;

	TArray<FPlanetMeshSection> MeshSections;
public:
	UPlanetLandscape();

public:
	void Initialize(FPlanetShapeGenerator inGenerator, FPlanetMeshSettings inMeshSettings, 
		URuntimeMeshComponentStatic& inMesh, FVector inLocalUp);
	void ConstructMesh(const FVector& NewLocation);
	void CalculateNoise();
	void OnCameraLocationUpdated(const FVector& NewLocation);
};
