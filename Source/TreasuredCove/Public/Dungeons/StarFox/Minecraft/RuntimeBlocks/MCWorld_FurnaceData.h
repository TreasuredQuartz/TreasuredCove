// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dungeons/StarFox/Minecraft/RuntimeBlocks/BlockRuntimeData.h"
#include "MCWorld_FurnaceData.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UMCWorld_FurnaceData : public UBlockRuntimeData
{
	GENERATED_BODY()
	
public:
	class UInventoryComponent* Inventory;
};
