// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherSystem/WeatherZone.h"

// Sets default values
AWeatherZone::AWeatherZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeatherZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeatherZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

