// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ASAmmo.generated.h"

class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoModifiedDelegate, float, Ammo, float, MaxAmmo);

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASAmmo : public UAttributeSet
{
	GENERATED_BODY()

	UASAmmo();
public:
	// Ammunition
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ammo")
	FGameplayAttributeData MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ammo")
	FGameplayAttributeData Ammo;

	//
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	//
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data) override;
	//
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;

	FOnAmmoModifiedDelegate OnAmmoModified;
};
