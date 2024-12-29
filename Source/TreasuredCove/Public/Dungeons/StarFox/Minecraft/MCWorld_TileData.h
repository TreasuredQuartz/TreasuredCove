// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTileData.h"
#include "MCWorld_TileData.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UMCWorld_TileData : public UGameplayTileData
{
	GENERATED_BODY()
	
public:
	TArray<UObject*> BlockData;
};
