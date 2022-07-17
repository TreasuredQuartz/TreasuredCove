// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayBuilding.h"
#include "GameplayResidence.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYTOWNS_API AGameplayResidence :
	public AGameplayBuilding
{
	GENERATED_BODY()
public:
	bool bIsLocked;
	bool bHasBedding;
	bool bHasFood;

	uint8 BeddingQuality;
	uint8 FoodQuality;

	TArray<FVector> Bedding;
	TArray<FVector> Pantry;

	void ReduceFatigue(AActor* User);
	void ReduceHunger(AActor* User);
};
