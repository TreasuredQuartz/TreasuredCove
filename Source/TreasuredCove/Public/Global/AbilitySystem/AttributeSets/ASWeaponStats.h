// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASCombat.h"
#include "ASWeaponStats.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHipAccuracyModified, float, SpreadMin, float, SpreadMax, float, SpreadIncreaseSpeed, float, SpreadDecreaseSpeed, float, SpreadDeltaMultiplier);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnADSAccuracyModified, float, Sway, float, HorizontalAccuracy, float, VerticalAccuracy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpreadAngleModified, float, SpreadAngle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHandlingModified, float, ReloadSpeed, float, DrawSpeed, float, ADSSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRangeModified, float, MinRange, float, MaxRange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageModified, float, MinDamage, float, MaxDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCoreAbilitiesModified, float, Cooldown, float, Cost, float, CastTime);

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASWeaponStats : public UASCombat
{
	GENERATED_BODY()

public:
	UASWeaponStats();
public:
	// Core stats that define how a gun acts when held, fired, etc

	// The unwieldiness of the weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadDecreaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadIncreaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData SpreadDeltaMultiplier;

	/* Accuracy Dependent Stats */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData Sway;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData HorizontalRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData VerticalRecoil;

	/* Handling Dpendent Stats */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	// FGameplayAttributeData Handling;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData ReloadSpeed;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData DrawSpeed;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData ADSSpeed;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData Mass;

	/* Damage Dependent Stats 
	* - Impact Power is effectively Damage
	*/

	// The range at which the projectile reaches maximum damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData MinRange;

	// The range at which the projectile disappears
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData MaxRange;

	// With the addition of buffs and other effects, Damage might need to be clamped at some point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData MinDamage;

	// With the addition of buffs and other effects, Damage might need to be clamped at some point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData MaxDamage;

	// Twice the diameter of the current bullet (In milimeters)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Combat")
	FGameplayAttributeData ImpactPower;

	// Ability Outputs

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ability")
	FGameplayAttributeData Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ability")
	FGameplayAttributeData Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Ability")
	FGameplayAttributeData CastTime;
public:
	bool ShouldInitProperty(bool FirstInit, FProperty* PropertyToInit) const override;
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data);

	FOnSpreadAngleModified OnSpreadAngleModified;
	FOnHipAccuracyModified OnHipAccuracyModified;
	FOnADSAccuracyModified OnADSAccuracyModified;
	FOnHandlingModified OnHandlingModified;
	FOnRangeModified OnRangeModified;
	FOnDamageModified OnDamageModified;
	FOnCoreAbilitiesModified OnCoreAbilitiesModified;
};
