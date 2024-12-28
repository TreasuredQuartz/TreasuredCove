// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/Interfaces/InventoryInterface.h"
#include "Dungeons/StarFox/Minecraft/RuntimeBlocks/BlockRuntimeData.h"
#include "MCWorld_ChestData.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UMCWorld_ChestData : public UBlockRuntimeData, public IInventoryInterface
{
	GENERATED_BODY()

public:
	UInventoryComponent* Inventory;
	virtual UInventoryComponent* GetInventoryComponent_Implementation() const override { return Inventory; };
	
};
