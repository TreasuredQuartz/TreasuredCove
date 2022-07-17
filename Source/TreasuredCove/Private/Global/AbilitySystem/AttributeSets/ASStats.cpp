// Fill out your copyright notice in the Description page of Project Settings.

#include "ASStats.h"
#include "GASystemComponent.h"
#include "GameplayAbilitiesModule.h"
#include "AbilitySystemGlobals.h"
#include "StaticLibrary.h"

void UASStats::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	FProperty* ExperienceProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Experience));

	FProperty* CharismaProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Charisma));
	FProperty* ConstitutionProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Constitution));
	FProperty* DexterityProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Dexterity));
	FProperty* IntelligenceProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Intelligence));
	FProperty* StrengthProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Strength));
	FProperty* WisdomProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Wisdom));

	NewValue = FMath::Clamp(NewValue, 0.f, NewValue);

	if (Attribute == ExperienceProperty)
	{
		OnExperienceModified.Broadcast(Experience.GetCurrentValue(), MaxExperience.GetCurrentValue());
	}
	else if (Attribute == CharismaProperty)
	{
		OnStatModified.Broadcast(Charisma.GetCurrentValue(), Constitution.GetCurrentValue(), Dexterity.GetCurrentValue(), Intelligence.GetCurrentValue(), Strength.GetCurrentValue(), Wisdom.GetCurrentValue());
	}
	else if (Attribute == ConstitutionProperty)
	{
		OnStatModified.Broadcast(Charisma.GetCurrentValue(), Constitution.GetCurrentValue(), Dexterity.GetCurrentValue(), Intelligence.GetCurrentValue(), Strength.GetCurrentValue(), Wisdom.GetCurrentValue());
	}
	else if (Attribute == DexterityProperty)
	{
		OnStatModified.Broadcast(Charisma.GetCurrentValue(), Constitution.GetCurrentValue(), Dexterity.GetCurrentValue(), Intelligence.GetCurrentValue(), Strength.GetCurrentValue(), Wisdom.GetCurrentValue()); 
	}
	else if (Attribute == IntelligenceProperty)
	{
		OnStatModified.Broadcast(Charisma.GetCurrentValue(), Constitution.GetCurrentValue(), Dexterity.GetCurrentValue(), Intelligence.GetCurrentValue(), Strength.GetCurrentValue(), Wisdom.GetCurrentValue()); 
	}
	else if (Attribute == StrengthProperty)
	{
		OnStatModified.Broadcast(Charisma.GetCurrentValue(), Constitution.GetCurrentValue(), Dexterity.GetCurrentValue(), Intelligence.GetCurrentValue(), Strength.GetCurrentValue(), Wisdom.GetCurrentValue()); 
	}
	else if (Attribute == WisdomProperty)
	{
		OnStatModified.Broadcast(Charisma.GetCurrentValue(), Constitution.GetCurrentValue(), Dexterity.GetCurrentValue(), Intelligence.GetCurrentValue(), Strength.GetCurrentValue(), Wisdom.GetCurrentValue()); 
	}

}

bool UASStats::ShouldInitProperty(bool FirstInit, FProperty* PropertyToInit) const
{
	//// Get all property members here that should be initialized based on other Attributes
	//FProperty* VitalityProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Vitality));
	//FProperty* VigorProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, Vigor));
	//FProperty* MentalEnergyProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, MentalEnergy));

	//FProperty* ConHealthBonusProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, ConHealthBonus));
	//FProperty* DexHealthBonusProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, DexHealthBonus));
	//FProperty* StrHealthBonusProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, StrHealthBonus));

	//FProperty* ConStaminaBonusProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, ConStaminaBonus));
	//FProperty* DexStaminaBonusProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, DexStaminaBonus));
	//FProperty* StrStaminaBonusProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, StrStaminaBonus));

	//FProperty* IntManaBonusProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, IntManaBonus));
	//FProperty* WisManaBonusProperty = FindFieldChecked<FProperty>(UASStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStats, WisManaBonus));

	//if (FirstInit)
	//{
	//	return PropertyToInit != VitalityProperty &&
	//		PropertyToInit != VigorProperty &&
	//		PropertyToInit != MentalEnergyProperty &&
	//		PropertyToInit != ConHealthBonusProperty &&
	//		PropertyToInit != DexHealthBonusProperty &&
	//		PropertyToInit != StrHealthBonusProperty &&
	//		PropertyToInit != ConStaminaBonusProperty &&
	//		PropertyToInit != DexStaminaBonusProperty &&
	//		PropertyToInit != StrStaminaBonusProperty &&
	//		PropertyToInit != IntManaBonusProperty &&
	//		PropertyToInit != WisManaBonusProperty;
	//}

	return true;
}
