// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetNoise.h"
#include "SimplexNoiseBPLibrary.h"

float FPlanetNoise::Evaluate(const FVector& Point)
{
	float noiseValue = 0;
	float frequency = Settings.BaseRoughness;
	float amplitude = 1;

	for (int i = 0; i < Settings.numLayers; ++i)
	{
		FVector noise = (Point * frequency + Settings.Center);
		float value = USimplexNoiseBPLibrary::SimplexNoise3D(noise.X, noise.Y, noise.Z, 1);
		noiseValue += (value + 1) * .5f * amplitude;
		frequency *= Settings.Roughness;
		amplitude *= Settings.Persistence;
	}

	noiseValue = FMath::Max(0.0f, noiseValue - Settings.minValue);

	return (noiseValue) * Settings.Strength;
}