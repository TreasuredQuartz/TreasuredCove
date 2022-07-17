// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayVoxel.h"
#include "WeatherInterface.h"
#include "WeatherZoneInfoStruct.h"
#include "AtmosphereVoxel.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API AAtmosphereVoxel : 
	public AGameplayVoxel
{
	GENERATED_BODY()

public:
	AAtmosphereVoxel();
	
public:
	/*
	int32 ChunkTotalElements;

	int32 ChunkLineElements;
	int32 ChunkLineElementsExt;

	int32 ChunkXIndex;
	int32 ChunkYIndex;
	*/
	float XMult;
	float YMult;
	float Weight;

	UFUNCTION(BlueprintNativeEvent)
		TArray<int32> CalcNoise();
public:
	FWeatherZoneInfo ZoneInfo;

	virtual void EnterVoxel() override;
	virtual void ExitVoxel() override;

	virtual void BeforeChunkGenerated() override;
	virtual void ChunkGenerating(FIntVector CurrentLocation, int32 Index) override;
	virtual void AfterChunkGenerated() override;
};
