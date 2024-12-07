// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeMacros.h"
#include "OnAttributeModifiedEvent.h"
#include "ASAmmo.generated.h"

class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASAmmo : public UAttributeSet
{
	GENERATED_BODY()

private:
	bool bAmmoZeroed;
	float AmmoBeforeAttributeChange;

private:
	UFUNCTION()
	void OnRep_Ammo(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxAmmo(const FGameplayAttributeData& OldValue);

public:
	UASAmmo();

	ATTRIBUTE_ACCESSORS(UASAmmo, Ammo);
	ATTRIBUTE_ACCESSORS(UASAmmo, MaxAmmo);

	// Ammunition
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmo, Category = "Attributes|Ammo")
	FGameplayAttributeData MaxAmmo;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Ammo, Category = "Attributes|Ammo")
	FGameplayAttributeData Ammo;

	//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data) override;
	//
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnAmmoModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnMaxAmmoModified;
	// Declared as mutable because pointer to set will be const
	mutable FOnAttributeModifiedEvent OnAmmoZeroed;
};
