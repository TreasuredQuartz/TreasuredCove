// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayManager.h"
#include "WeatherInterface.h"
#include "Enumerations\AtmosphereType.h"
#include "Structs\WeatherZoneInfoStruct.h"
#include "AtmosphereManager.generated.h"

class UWeatherComponent;

UCLASS()
class TREASUREDCOVE_API AAtmosphereManager : 
	public AGameplayManager, 
	public IWeatherInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAtmosphereManager();

	UWeatherComponent* PlayerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weather")
	AActor* SunSky_Ref;

	float WorldTemperature;
	float WorldTemperatureVarience;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetAtmosphere(EAtmosphereType InAtmosphere);
	void EnterZone(FWeatherZoneInfo ZoneInfo);
	void ExitZone();

public:
	UWeatherComponent* GetWeatherComponent() { return PlayerComponent; };
};
