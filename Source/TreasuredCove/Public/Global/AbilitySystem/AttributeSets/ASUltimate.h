// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeMacros.h"
#include "OnAttributeModifiedEvent.h"
#include "ASUltimate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUltimateChargeModified, float, UltimateCharge, float, MaxUltimate);

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASUltimate : public UAttributeSet
{
	GENERATED_BODY()
private:
	bool bUltimateChargeZeroed;
	float UltimateChargeBeforeAttributeChange;

private:
	UFUNCTION()
	void OnRep_UltimateCharge(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxUltimateCharge(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_UltimateChargeRegenRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_UltimateChargeRegenDelay(const FGameplayAttributeData& OldValue);
	
public:
	UASUltimate();

	/* All Ultimate Attributes */

	ATTRIBUTE_ACCESSORS(UASUltimate, UltimateCharge);
	ATTRIBUTE_ACCESSORS(UASUltimate, MaxUltimateCharge);
	ATTRIBUTE_ACCESSORS(UASUltimate, UltimateChargeRegenRate);
	ATTRIBUTE_ACCESSORS(UASUltimate, UltimateChargeRegenDelay);

	// Ultimate charge cannot exceed, and becomes available after reaching, this amount.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxUltimateCharge, Category = "Attributes|Ultimate")
	FGameplayAttributeData MaxUltimateCharge;
	// The current Ultimate charge.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UltimateCharge, Category = "Attributes|Ultimate")
	FGameplayAttributeData UltimateCharge;
	// The Rate per Second that Ultimate charge is generated passively.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UltimateChargeRegenRate, Category = "Attributes|Ultimate")
	FGameplayAttributeData UltimateChargeRegenRate;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_UltimateChargeRegenDelay, Category = "Attributes|Ultimate")
	FGameplayAttributeData UltimateChargeRegenDelay;

public:
	//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data) override;
	//
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnUltimateChargeModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnMaxUltimateChargeModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnUltimateChargeRegenRateModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnUltimateChargeRegenDelayModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnUltimateChargeZeroed;
};
