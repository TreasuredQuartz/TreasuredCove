// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ASCharacteristics.generated.h"

// #define DECLARE_NAMED_STAT_MODIFIER_GETTER(FunctionName, Property) FORCEINLINE float FunctionName##() const { return (Property##.GetCurrentValue() - 13) / 2; }

UENUM(BlueprintType)
enum ECharacteristic : uint8 {
	Charisma,
	Constitution,
	Dexterity,
	Intelligence,
	Strength,
	Wisdom,
	Characteristic_MAX
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExperienceModifiedDelegate, float, Experience, float, MaxExperience);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPointsModifiedDelegate, float, StatPoints, float, SkillPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacteristicModifiedDelegate, ECharacteristic, Characteristic, float, NewAmount);

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UASCharacteristics : 
	public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Stats")
	FGameplayAttributeData Charisma;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Stats")
	FGameplayAttributeData Constitution;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Stats")
	FGameplayAttributeData Dexterity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Stats")
	FGameplayAttributeData Intelligence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Stats")
	FGameplayAttributeData Strength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Stats")
	FGameplayAttributeData Wisdom;

	// Bonuses
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Bonus")
	FGameplayAttributeData ConHealthBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Bonus")
	FGameplayAttributeData DexHealthBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Bonus")
	FGameplayAttributeData StrHealthBonus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Bonus")
	FGameplayAttributeData ConStaminaBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Bonus")
	FGameplayAttributeData DexStaminaBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Bonus")
	FGameplayAttributeData StrStaminaBonus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Bonus")
	FGameplayAttributeData IntManaBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Bonus")
	FGameplayAttributeData WisManaBonus;*/

	// Leveling and Leveling currencies
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Level")
	FGameplayAttributeData StatPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Level")
	FGameplayAttributeData SkillPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes|Level")
	FGameplayAttributeData Level;*/

	/* DECLARE_NAMED_STAT_MODIFIER_GETTER(GetChaModifier, Charisma);
	DECLARE_NAMED_STAT_MODIFIER_GETTER(GetConModifier, Constitution);
	DECLARE_NAMED_STAT_MODIFIER_GETTER(GetDexModifier, Dexterity);
	DECLARE_NAMED_STAT_MODIFIER_GETTER(GetIntModifier, Intelligence);
	DECLARE_NAMED_STAT_MODIFIER_GETTER(GetStrModifier, Strength);
	DECLARE_NAMED_STAT_MODIFIER_GETTER(GetWisModifier, Wisdom); */

	/*FORCEINLINE float GetMaxHealthWithBonuses()
	{
		return MaxHealth.GetCurrentValue() + (ConHealthBonus.GetCurrentValue() + StrHealthBonus.GetCurrentValue() + DexHealthBonus.GetCurrentValue());
	}
	FORCEINLINE float GetMaxStaminaWithBonuses()
	{
		return MaxStamina.GetCurrentValue() + (ConStaminaBonus.GetCurrentValue() + StrStaminaBonus.GetCurrentValue() + DexHealthBonus.GetCurrentValue());
	}
	FORCEINLINE float GetMaxManaWithBonuses()
	{
		return MaxMana.GetCurrentValue() + (WisManaBonus.GetCurrentValue() + IntManaBonus.GetCurrentValue());
	}*/

	virtual bool ShouldInitProperty(bool FirstInit, FProperty* PropertyToInit) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	FOnExperienceModifiedDelegate OnExperienceModified;
	FOnPointsModifiedDelegate OnPointsModified;
	FOnCharacteristicModifiedDelegate OnCharacteristicModified;
};
