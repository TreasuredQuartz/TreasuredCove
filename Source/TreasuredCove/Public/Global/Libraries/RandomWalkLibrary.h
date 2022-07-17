// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RandomWalkLibrary.generated.h"

/** A Random walk noise creator.
 * 
 */
UCLASS()
class TREASUREDCOVE_API URandomWalkLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Generates a 2D 'map' with counts of footsteps
	* at X, Y coordinates.
	* 
	* Returns a 2D Array that can be accessed using
	* - index = x + (y * x_size) or an unintialized
	* array if any input is less than 1.
	* 
	* Should be noted that the minimum coordinate is 0 
	* and the maximum coord will be height/width - 1.
	*/
	UFUNCTION(BlueprintCallable, Category = "RandomWalk")
	static TArray<int32> GenerateRandomWalk2D(uint8 height, uint8 width, uint8 maxSteps, int32 RandomSeed);

};
