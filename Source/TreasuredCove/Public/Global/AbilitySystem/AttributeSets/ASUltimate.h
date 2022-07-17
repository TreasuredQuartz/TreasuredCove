// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ASUltimate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUltimateChargeModified, float, UltimateCharge, float, MaxUltimate);

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASUltimate : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UASUltimate();

	// Ultimate charge cannot exceed, and becomes available after reaching, this amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ultimate")
	FGameplayAttributeData MaxUltimateCharge;
	// The current Ultimate charge
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ultimate")
	FGameplayAttributeData UltimateCharge;
	// The Rate per Second that Ultimate charge is generated passively
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ultimate")
	FGameplayAttributeData UltimateChargeRate;

	//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data) override;
	//
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

	FOnUltimateChargeModified OnUltimateChargeModified;
};
