// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlanetMeshSettings.h"
#include "PlanetNoiseSettings.h"
#include "PlanetShapeSettings.generated.h"

USTRUCT(BlueprintType)
struct FPlanetNoiseLayer
{
	GENERATED_BODY()

	FPlanetNoiseLayer() : bIsEnabled(false), noiseSettings(FPlanetNoiseSettings())
	{};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator")
	bool bIsEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator")
	FPlanetNoiseSettings noiseSettings;

public:
	static bool isMemberName(FName Name)
	{
		return (Name == "bIsEnabled") || FPlanetNoiseSettings::isMemberName(Name);
	}
};

USTRUCT(BlueprintType)
struct PROCEDURALPLANETPLUGIN_API FPlanetShapeSettings
{
	GENERATED_BODY()

	FPlanetShapeSettings() : MeshSettings(FPlanetMeshSettings())
	{
		noiseLayers.SetNum(1);
	};
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator")
	TArray<FPlanetNoiseLayer> noiseLayers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator")
	FPlanetMeshSettings MeshSettings;

public:
	static bool isMemberName(FName Name)
	{
		return (Name == "radius") || FPlanetNoiseLayer::isMemberName(Name) || FPlanetMeshSettings::isMemberName(Name);
	}
};
