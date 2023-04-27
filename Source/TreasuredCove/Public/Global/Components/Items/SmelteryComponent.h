// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/Components/Items/CraftingToolComponent.h"
#include "SmelteryComponent.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API USmelteryComponent : public UCraftingToolComponent
{
	GENERATED_BODY()

private:
	UObject* Smeltery;
	UObject* FuelTank;
	UObject* Drain;
};
