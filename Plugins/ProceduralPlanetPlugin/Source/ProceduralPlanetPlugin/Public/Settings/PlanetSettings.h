// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlanetShapeSettings.h"
#include "PlanetMaterialSettings.h"
#include "PlanetSettings.generated.h"

USTRUCT(BlueprintType)
struct PROCEDURALPLANETPLUGIN_API FPlanetSettings
{
	GENERATED_BODY()

	FPlanetSettings() : 
		ShapeSettings(FPlanetShapeSettings()),
		MaterialSettings(FPlanetMaterialSettings())
	{};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator")
		FPlanetShapeSettings ShapeSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Shape Generator")
		FPlanetMaterialSettings MaterialSettings;
};
