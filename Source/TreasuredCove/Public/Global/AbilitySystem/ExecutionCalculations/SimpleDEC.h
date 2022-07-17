// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SimpleDEC.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API USimpleDEC : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

private:
	USimpleDEC();
	FProperty* HealthProperty;
	FGameplayEffectAttributeCaptureDefinition HealthDef;

public:
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;
};
