// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlanetLODSettings.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROCEDURALPLANETPLUGIN_API FPlanetLODSettings
{
	GENERATED_BODY()

public:
	FPlanetLODSettings() 
	{
		LevelsOfDetail = {
			(INFINITY),
			(1.f),
			(0.95f),
			(0.9f),
			(0.8f),
			(0.7f),
			(0.6f),
			(0.5f),
			(0.4f),
			(0.3f),
			(0.2f),
			(0.1f)
		};
	};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Level Of Detail")
	TArray<float> LevelsOfDetail;
	
public:
	static bool isMemberName(FName Name)
	{
		return (Name == "LevelsOfDetail");
	}
};
