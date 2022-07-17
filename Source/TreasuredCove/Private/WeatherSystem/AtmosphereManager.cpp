// Fill out your copyright notice in the Description page of Project Settings.


#include "AtmosphereManager.h"

#include "Particles/ParticleSystemComponent.h"

#include "WeatherComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAtmosphereManager::AAtmosphereManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAtmosphereManager::BeginPlay()
{
	Super::BeginPlay();

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Pawn->GetClass()->ImplementsInterface(UWeatherInterface::StaticClass()))
	{
		PlayerComponent = Cast<IWeatherInterface>(Pawn)->GetWeatherComponent();
	}

	SetAtmosphere(EAtmosphereType::Default);
}

// Called every frame
void AAtmosphereManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAtmosphereManager::SetAtmosphere(EAtmosphereType InAtmosphere)
{
	if(SunSky_Ref->GetClass()->ImplementsInterface(UWeatherInterface::StaticClass()))
	{
		Cast<IWeatherInterface>(SunSky_Ref)->SetAtmosphere(InAtmosphere);
	}
}

void AAtmosphereManager::EnterZone(FWeatherZoneInfo ZoneInfo)
{
	if(PlayerComponent)
		PlayerComponent->WeatherParticle->SetTemplate(ZoneInfo.WeatherZoneParticleSystem);
	SetAtmosphere(ZoneInfo.AtmosphereSetting);
}

void AAtmosphereManager::ExitZone()
{
	if (PlayerComponent)
		PlayerComponent->WeatherParticle->SetTemplate(nullptr);
	SetAtmosphere(EAtmosphereType::Default);
}

