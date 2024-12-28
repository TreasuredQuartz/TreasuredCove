// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeatherSystem/Enumerations/AtmosphereType.h"
#include "WeatherSystem/Structs/WeatherZoneInfoStruct.h"
#include "WeatherInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeatherInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TREASUREDCOVE_API IWeatherInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual class UWeatherComponent* GetWeatherComponent() = 0;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category= "Weather")
	void SetAtmosphere(EAtmosphereType InAtmosphere);

	virtual void EnterZone(FWeatherZoneInfo ZoneInfo) = 0;
	virtual void ExitZone() = 0;
};
