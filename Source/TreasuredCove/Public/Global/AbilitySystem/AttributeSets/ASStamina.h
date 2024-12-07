// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeMacros.h"
#include "OnAttributeModifiedEvent.h"
#include "ASStamina.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASStamina : public UAttributeSet
{
	GENERATED_BODY()

private:
	bool bStaminaZeroed;
	float StaminaBeforeAttributeChange;

private:
	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_StaminaRegenDelay(const FGameplayAttributeData& OldValue);

public:
	// Constructor
	UASStamina();

	ATTRIBUTE_ACCESSORS(UASStamina, Stamina);
	ATTRIBUTE_ACCESSORS(UASStamina, MaxStamina);
	ATTRIBUTE_ACCESSORS(UASStamina, StaminaRegenRate);
	ATTRIBUTE_ACCESSORS(UASStamina, StaminaRegenDelay);

	// All Stamina properties
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStamina, Category = "Attributes|Stamina")
	FGameplayAttributeData MaxStamina;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina, Category = "Attributes|Stamina")
	FGameplayAttributeData Stamina;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegenRate, Category = "Attributes|Stamina")
	FGameplayAttributeData StaminaRegenRate;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StaminaRegenDelay, Category = "Attributes|Stamina")
	FGameplayAttributeData StaminaRegenDelay;

public:
	//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	//
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnStaminaModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnMaxStaminaModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnStaminaRegenRateModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnStaminaRegenDelayModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnStaminaZeroed;
};
