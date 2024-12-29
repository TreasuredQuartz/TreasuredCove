// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherSystem/AtmosphereVoxel.h"
#include "WeatherSystem/WeatherComponent.h"
#include "GameplayManager.h"
#include "SimplexNoiseBPLibrary.h"

AAtmosphereVoxel::AAtmosphereVoxel()
{
	// Root Added in AGamneplayVoxel
	/*USceneComponent* Root =
		CreateDefaultSubobject<USceneComponent>(TEXT("Root"));*/
}

void AAtmosphereVoxel::EnterVoxel()
{
	if (ManagerRef->GetClass()->ImplementsInterface(UWeatherInterface::StaticClass()))
	{
		Cast<IWeatherInterface>(ManagerRef)->EnterZone(ZoneInfo);
	}
}

void AAtmosphereVoxel::ExitVoxel()
{
	if (ManagerRef->GetClass()->ImplementsInterface(UWeatherInterface::StaticClass()))
	{
		Cast<IWeatherInterface>(ManagerRef)->ExitZone();
	}
}

void AAtmosphereVoxel::BeforeChunkGenerated()
{
	TArray<int32> Temperature = CalcNoise();
}

void AAtmosphereVoxel::ChunkGenerating(const FIntVector& CurrentLocation, int32 Index) 
{

}

void AAtmosphereVoxel::AfterChunkGenerated() 
{

}

TArray<int32> AAtmosphereVoxel::CalcNoise_Implementation()
{
	TArray<int32> Value;

	float LocX = 0;
	float LocY = 0;

	for (int y = 0; y < ChunkLineElements; y++)
	{
		LocY = (y + (ChunkIndex.Y * ChunkLineElements)) * YMult;

		for (int x = 0; x < ChunkLineElements; x++)
		{
			LocX = (x + (ChunkIndex.X * ChunkLineElements)) * XMult;

			float Noise0 = USimplexNoiseBPLibrary::SimplexNoise2D(LocX / 40, LocY / 40);
			float Noise1 = USimplexNoiseBPLibrary::SimplexNoise2D(LocX / 10, LocY / 10);
			float Noise2 = USimplexNoiseBPLibrary::SimplexNoise2D(LocX / 256, LocY / 256);

			Noise0 = (Noise0 + 1) / 2;
			Noise0 = Noise0 * 12;
			Noise1 = (Noise1 + 1) / 2;
			Noise1 = Noise1 * 4;
			Noise2 = (Noise2 + 1) / 2;
			Noise2 = Noise2 * 32;

			float Noise3 = FMath::Clamp<float>(USimplexNoiseBPLibrary::SimplexNoise2D(LocX / 10, LocY / 10) * 15.f, 0.f, 5.f);

			int32 FinalNoise = floor(((Noise0 + Noise1 + Noise2 + Noise3)) * Weight);
			FinalNoise = FMath::Clamp<int32>(FinalNoise, 0, ChunkTotalElements);
			Value.Add(FinalNoise);
		}
	}

	return Value;
}