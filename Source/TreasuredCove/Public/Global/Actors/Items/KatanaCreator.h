// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProceduralMeshSectionStruct.h"
#include "KatanaCreator.generated.h"

UCLASS()
class TREASUREDCOVE_API UPart : public UObject
{
	GENERATED_BODY()

public:
	UPart();

private:
	// Recursive option to help simplify complex parts
	TArray<UPart*> InnerParts;

	// The Dimensions of this part. 
	// If containing InnerParts, then this will be used as
	// As a cheaper LOD/SweepTrace.
	TArray<FProceduralMeshSection> Dimensions;
};

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UKatanaCreator : public UObject
{
	GENERATED_BODY()

public:
	UKatanaCreator();

private:
	UPart* Handle;
	UPart* Guard;
	UPart* Blade;

public:
	void GenerateKatana();
};
