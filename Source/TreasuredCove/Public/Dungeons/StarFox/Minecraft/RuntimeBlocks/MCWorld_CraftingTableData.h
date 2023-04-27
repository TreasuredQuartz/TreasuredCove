// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dungeons/StarFox/Minecraft/RuntimeBlocks/BlockRuntimeData.h"
#include "MCWorld_CraftingTableData.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UMCWorld_CraftingTableData : public UBlockRuntimeData
{
	GENERATED_BODY()
	
public:
	class UInventoryComponent* Inventory;
};
