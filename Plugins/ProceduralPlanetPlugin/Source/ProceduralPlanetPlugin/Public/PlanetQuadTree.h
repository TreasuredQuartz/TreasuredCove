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
	UPlanetQuadTree **Children;

	const FVector& PlayerPosition;
	FVector position;
	FVector axisA;
	FVector axisB;

	float localRadius;

	uint8 detailLevel;
public:
	UPlanetQuadTree() : Settings(FPlanetMeshSettings()), PlayerPosition(FVector()) {};

	UPlanetQuadTree(FPlanetMeshSettings inSettings, UPlanetQuadTree** inChildren, FVector inPlayerPosition, FVector inPosition, FVector inAxisA, FVector inAxisB,
		float inLocalRadius, uint8 inDetailLevel)
		:
		Settings(inSettings),
		Children(inChildren),
		PlayerPosition(inPlayerPosition),
		position(inPosition), 
		axisA(inAxisA), 
		axisB(inAxisB), 
		localRadius(inLocalRadius), 
		detailLevel(inDetailLevel)
	{};

public:
	void ConstructQuadTree();
	void CalculateMesh(TArray<FVector>& vertices, TArray<int>& triangles, TArray<FColor>& colors, int triangleOffset);
	TArray<UPlanetQuadTree*> GetVisibleChildren();

	// Debugging
public:
	FString ToString();
	FColor GetDebugColorForRegions();
};
