// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "PlanetNoiseSettings.h"
#include "PlanetNoiseInterface.h"
#include "PlanetNoise.generated.h"

/** This struct inherits from an interface.
 * This is a big no-no from UE4's Unreal Header Tool (UHT)
 * And to achieve this, according to TheJamah
 * on forums.unrealengine, you can skip these
 * lines using a pre-processsor block.
 */
USTRUCT()
struct FPlanetNoise
#if CPP
	: public IPlanetNoiseInterface
#endif
{
	GENERATED_BODY()

public:
	FPlanetNoise() : Settings(FPlanetNoiseSettings()) {};
	FPlanetNoise(FPlanetNoiseSettings inSettings)
	{
		Settings = inSettings;
	};

	FPlanetNoiseSettings Settings;

	virtual float Evaluate(const FVector& Point) override;
};
