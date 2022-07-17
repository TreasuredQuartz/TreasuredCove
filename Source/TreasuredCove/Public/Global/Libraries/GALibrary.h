// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilityInfoStruct.h"
#include "GALibrary.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGALibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Editor")
	static void GetEditorViewLocRot(FVector& OutLocation, FRotator& OutRotation);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	static void ApplyGESpecHandleToTargetDataSpecsHandle(const FGameplayEffectSpecHandle& GESpecHandle, const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static bool SwitchOnAttributeName(FString InAttributeName, EAbilityCostType& OutCostType);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static bool CheckAttributeCost(class UAbilitySystemComponent* InAbilitySystem, FString InAttributeName, float Cost);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static float GetAttributeCurrentValue(class UAbilitySystemComponent* InAbilitySystem, FString InAttributeName, bool& OutDidSucceed);
};
