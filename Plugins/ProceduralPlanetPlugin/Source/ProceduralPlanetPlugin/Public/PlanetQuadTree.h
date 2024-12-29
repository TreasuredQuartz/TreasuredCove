// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Settings/PlanetMeshSettings.h"
#include "Generators/PlanetShapeGenerator.h"

/**
 * 
 */
class PROCEDURALPLANETPLUGIN_API UPlanetQuadTree
{
public:
// private:
	FPlanetMeshSettings Settings;
	TArray<UPlanetQuadTree*> Children;

	FVector PlayerPosition;
	FVector ActorLocation;
	FVector position;

// private:
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
	void DestructQuadTree();
	void CalculateMesh(TArray<FVector>& vertices, TArray<int>& triangles, TArray<FColor>& colors, int triangleOffset) const;
	TArray<UPlanetQuadTree*> GetVisibleChildren();
	bool CheckLOD(const FVector& NewLocation);
	bool CheckLODDistance() const;
	bool ShouldIncreaseLOD(const double InDistance) const;
	bool ShouldDecreaseLOD(const double InDistance) const;
	double GetPlayerDistance() const;

public:
	// Debugging //
	FColor GetDebugColorForRegions() const;
	FString ToString() const;
};
