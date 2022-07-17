// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AdvancedDEC.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UAdvancedDEC : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
private:
	UAdvancedDEC();
public:
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
