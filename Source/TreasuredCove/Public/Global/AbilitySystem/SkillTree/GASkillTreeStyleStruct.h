// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASkillTreeStyleStruct.generated.h"

UENUM(BlueprintType)
enum class EGridTileType : uint8
{
	Absolute,	// Pre-defined Style
	Square,		// Borderlands Style
	Hex,		// Civilization Style
	Octo,		// Octagons
	MAX			UMETA(Hidden)
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FGASkillTreeStyleStruct 
{
	GENERATED_BODY()
	
public:

};
