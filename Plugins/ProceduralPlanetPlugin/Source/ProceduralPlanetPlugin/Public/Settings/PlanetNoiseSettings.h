// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlanetNoiseSettings.generated.h"

USTRUCT(BlueprintType)
struct PROCEDURALPLANETPLUGIN_API FPlanetNoiseSettings
{
	GENERATED_BODY()
public:
	FPlanetNoiseSettings() :
		Center(FVector(0, 0, 0)),
		BaseRoughness(1),
		Roughness(2),
		Persistence(.5f),
		Strength(1),
		minValue(0),
		numLayers(1)
	{};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape | Noise")
		FVector Center;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape | Noise")
		float BaseRoughness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape | Noise")
		float Roughness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape | Noise")
		float Persistence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape | Noise")
		float Strength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape | Noise")
		float minValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape | Noise")
		int numLayers;

public:
	static bool isMemberName(FName Name)
	{
		return (Name == "Center") ||
			(Name == "BaseRoughness") ||
			(Name == "Roughness") ||
			(Name == "Persistence") ||
			(Name == "Strength") ||
			(Name == "minValue") ||
			(Name == "numLayers");
	}
};