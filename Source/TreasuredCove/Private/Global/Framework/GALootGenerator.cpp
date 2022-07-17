// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Framework/GALootGenerator.h"

float UGAWeightedLootTable::CalculateSum()
{
	float Total = 0.0f;
	TArray<float> Values;

	LootTable.GenerateValueArray(Values);
	if (!LootTable.IsEmpty())
	{
		for (float Value : Values)
		{
			Total += Value;
		}
	}

	return Total;
}

void UGAWeightedLootTable::ChooseLoot()
{

}

AActor* UGAWeightedLootTable::SpawnLoot()
{
	return nullptr;
}
