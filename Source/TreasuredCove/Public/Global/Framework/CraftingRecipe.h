// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemKeyStruct.h"
#include "CraftingRecipe.generated.h"

class AActor;

/**
 * 
 */
USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FCraftingRecipe
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Crafting")
	TArray<FItemKey> Ingredients;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Crafting")
	TArray<FItemKey> Result;
};
