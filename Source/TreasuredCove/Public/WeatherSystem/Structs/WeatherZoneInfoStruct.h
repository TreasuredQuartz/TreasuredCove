// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeatherSystem/Enumerations/AtmosphereType.h"
#include "WeatherZoneInfoStruct.generated.h"

USTRUCT(BlueprintType)
struct FWeatherZoneInfo
{
	GENERATED_BODY()

public:
	FWeatherZoneInfo() : WeatherZoneParticleSystem(nullptr),
		AtmosphereSetting(EAtmosphereType::NoChange)
	{}
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	class UParticleSystem* WeatherZoneParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EAtmosphereType AtmosphereSetting;
};
