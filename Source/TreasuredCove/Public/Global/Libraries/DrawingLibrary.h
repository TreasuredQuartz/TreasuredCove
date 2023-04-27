// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DrawingLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UDrawingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Utility function for converting an array to a texture.
	// - Used for dynamic bullet icons
	UFUNCTION(BlueprintCallable, Category = "Drawing")
	static UTexture2D* CreateTextureFromArray(const TArray<FColor>& Colors);
};
