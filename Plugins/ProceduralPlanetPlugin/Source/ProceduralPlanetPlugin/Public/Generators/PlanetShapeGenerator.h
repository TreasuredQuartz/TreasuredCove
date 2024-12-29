// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MinMax.h"
#include "Settings/PlanetShapeSettings.h"
#include "PlanetShapeGenerator.generated.h"

class IPlanetNoiseInterface;

/**
 *  
 */
USTRUCT(BlueprintType)
struct FPlanetShapeGenerator
{
	GENERATED_BODY()
public:
	FPlanetShapeGenerator() : Settings(FPlanetShapeSettings()), elevationMinMax(FMinMax()) { PlanetNoises.Init(nullptr, Settings.noiseLayers.Num()); };
	FPlanetShapeGenerator(FPlanetShapeSettings inSettings);
	void UpdateSettings(FPlanetShapeSettings inSettings);

	// void Initialize(FPlanetShapeSettings* inSettings);

public:
	TArray<IPlanetNoiseInterface*> PlanetNoises;
	FPlanetShapeSettings Settings;
	FMinMax elevationMinMax;

public:
	FVector CalculatePointOnPlanet(FVector PointOnUnitSphere);

private:
	/// #begin depreciated section				///
	/// @desc associated functions for			///
	/// fibonacci sphere moved here.			///
	/// @todo move functions to different class ///
	
	/** Mesh Generation
	* Points = Vertices
	* Triangles
	* UVs
	* Normals
	*/

	// fibonacci points
	void GeneratePoints(int numPoints, TArray<FVector>& Points);

	// Triangles formed based on distance to neighbors
	// (intended for fibonacci points)
	void GenerateTriangles(TArray<FVector>const& Vertices,
		TArray<int>& Triangles);

	// 2D fibonacci points mirrored across Z-axis
	void GenerateUVs(TArray<FVector>const& Points,
		TArray<FVector2D>& inUVs);

	// Vertices are returned normalized inside of the 
	// normals array. (Points remain unchanged)
	void GenerateNormals(TArray<FVector>const& Points,
		TArray<FVector>& inNormals);

	/** Project 3D sphere point onto 
	* infinte plane.
	*/
	FVector2D SterographicProjection(const FVector& xyz);

	/** Calls "SterographicProjection()" on every point
	* of an array.
	*/
	TArray<FVector2D> MassSterographicProjection(TArray<FVector>const& xyz);

	// Returns whether vertices are clockwise or not
	// (not mine)
	bool IsClockwise(TArray<FVector>const& vertices,
		int index1, int index2, int index3);

	// Actual function for triangles of fibonacci
	// points. (Not mine)
	void AddTrianglesInClockwiseDirection(TArray<int>& Triangles, TArray<FVector>const& Vertices,
		int index1, int index2, int index3);



	/// #end depreciated section				///
};
