// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlanetLODSettings.h"
#include "PlanetMeshSettings.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROCEDURALPLANETPLUGIN_API FPlanetMeshSettings
{
	GENERATED_BODY()

public:
	FPlanetMeshSettings() : 
		resolutionPerChunk(8), 
		radius(1000000),
		LODSettings(FPlanetLODSettings()) {};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator | Mesh")
	int resolutionPerChunk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator | Mesh")
	float radius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator | Mesh")
	FPlanetLODSettings LODSettings;

public:
	static bool isMemberName(FName Name)
	{
		return (Name == "resolutionPerChunk") || (Name == "radius") || FPlanetLODSettings::isMemberName(Name);
	}
};
