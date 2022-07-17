// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GALootGenerator.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGAWeightedLootTable : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot Table", meta = (AllowPrivateAccess = "true"))
	TMap<FName, float> LootTable;
	float LootSum = 0.0f;

	// Runtime Calculation of totaled weight within table
	UFUNCTION(BlueprintCallable, Category = "Loot Table")
	float CalculateSum();
	// Decides loot from table based on weight
	UFUNCTION(BlueprintCallable, Category = "Loot Table")
	void ChooseLoot();
public:
	// Random weighted spawn from table
	UFUNCTION(BlueprintCallable, Category = "Loot Table")
	AActor* SpawnLoot();
};
