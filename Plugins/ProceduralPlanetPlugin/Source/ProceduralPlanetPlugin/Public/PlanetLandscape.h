// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "PlanetMeshSettings.h"
#include "PlanetShapeGenerator.h"
#include "PlanetLandscape.generated.h"

// 100,000
// constexpr int PlanetSize = 100000;

class UProceduralMeshComponent;
class UPlanetQuadTree;

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

private:
	UPlanetQuadTree* QuadTree;
	UProceduralMeshComponent* Mesh;

	FVector localUp;
	FVector axisA;
	FVector axisB;

	bool bInitialized;

	TArray<FVector> vertices;
	TArray<int> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uvs;
	TArray<FColor> colors;
public:
	UPlanetLandscape();

public:
	void Initialize(FPlanetShapeGenerator inGenerator, FPlanetMeshSettings inMeshSettings, 
		UProceduralMeshComponent& inMesh, FVector inLocalUp);
	void ConstructMesh(const FVector& NewLocation);
	void CalculateNoise();
	void OnCameraLocationUpdated(const FVector& NewLocation);
};
