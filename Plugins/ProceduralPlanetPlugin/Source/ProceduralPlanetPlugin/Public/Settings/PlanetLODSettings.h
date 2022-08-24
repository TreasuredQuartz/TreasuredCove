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
			(1000000.f),
			(10000.f),
			(1000.f),
			(100.f),
			(50.f),
			(25.f),
			(20.f),
			(15.f),
			(10.f),
			(5.f),
			(1.f)
		};
	};

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Level Of Detail")
	TArray<double> LevelsOfDetail;
	
public:
	static bool isMemberName(FName Name)
	{
		return (Name == "LevelsOfDetail");
	}
};
