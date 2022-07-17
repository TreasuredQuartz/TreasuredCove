// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FluidCubeStruct.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FFluidCubeStruct
{
	GENERATED_BODY()

	int size;
	float dt;
	float diff;
	float visc;

	float* s;
	float* density;

	float* Vx;
	float* Vy;
	float* Vz;

	float* VxO;
	float* VyO;
	float* VzO;

public:	
	// Sets default values
	FFluidCubeStruct() {};

};


