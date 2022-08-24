// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "PlanetMeshSettings.h"
#include "PlanetShapeGenerator.h"

/**
 * 
 */
class PROCEDURALPLANETPLUGIN_API UPlanetQuadTree
{
private:
	FPlanetMeshSettings Settings;
	TArray<UPlanetQuadTree*> Children;

	FVector PlayerPosition;
	FVector position;
	FVector axisA;
	FVector axisB;

	float localRadius;

	uint8 detailLevel;

public:
	uint8 GetDetailLevel();

public:
	UPlanetQuadTree() : Settings(FPlanetMeshSettings()), PlayerPosition(FVector::ZeroVector) {};

	UPlanetQuadTree(FPlanetMeshSettings& inSettings, const FVector& inPlayerPosition, FVector& inPosition, FVector& inAxisA, FVector& inAxisB,
		float inLocalRadius, uint8 inDetailLevel)
		:
		Settings(inSettings),
		Children(TArray<UPlanetQuadTree*>()),
		PlayerPosition(inPlayerPosition),
		position(inPosition), 
		axisA(inAxisA), 
		axisB(inAxisB), 
		localRadius(inLocalRadius), 
		detailLevel(inDetailLevel)
	{};

public:
	void ConstructQuadTree();
	void CalculateMesh(TArray<FVector>& vertices, TArray<int>& triangles, TArray<FColor>& colors, int triangleOffset) const;
	TArray<UPlanetQuadTree*> GetVisibleChildren();
	bool CheckLOD(const FVector& NewLocation);

public:
	// Debugging //
	FColor GetDebugColorForRegions() const;
	FString ToString() const;
};
