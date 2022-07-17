// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlanetMaterialSettings.h"

#include "MinMax.h"
#include "PlanetMaterialGenerator.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROCEDURALPLANETPLUGIN_API FPlanetMaterialGenerator
{
	GENERATED_BODY()
public:
	FPlanetMaterialSettings Settings;

public:
	FPlanetMaterialGenerator() : Settings(FPlanetMaterialSettings()) {};

	FPlanetMaterialGenerator(FPlanetMaterialSettings inSettings) : Settings(inSettings) {};

public:
	void UpdateSettings(FPlanetMaterialSettings inSettings);
	void UpdateElevation(FMinMax MM);
};
