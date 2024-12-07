// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeMacros.h"
#include "OnAttributeModifiedEvent.h"
#include "ASMana.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASMana : public UAttributeSet
{
	GENERATED_BODY()

private:
	bool bManaZeroed;
	float ManaBeforeAttributeChange;

private:
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ManaRegenRate(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ManaRegenDelay(const FGameplayAttributeData& OldValue);

public:
	// Constructor
	UASMana();

	ATTRIBUTE_ACCESSORS(UASMana, Mana);
	ATTRIBUTE_ACCESSORS(UASMana, MaxMana);
	ATTRIBUTE_ACCESSORS(UASMana, ManaRegenRate);
	ATTRIBUTE_ACCESSORS(UASMana, ManaRegenDelay);

	// All Mana properties
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes|Mana")
	FGameplayAttributeData MaxMana;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes|Mana")
	FGameplayAttributeData Mana;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegenRate, Category = "Attributes|Mana")
	FGameplayAttributeData ManaRegenRate;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegenDelay, Category = "Attributes|Mana")
	FGameplayAttributeData ManaRegenDelay;

public:
	//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	//
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnManaModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnMaxManaModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnManaRegenRateModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnManaRegenDelayModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnManaZeroed;
};
