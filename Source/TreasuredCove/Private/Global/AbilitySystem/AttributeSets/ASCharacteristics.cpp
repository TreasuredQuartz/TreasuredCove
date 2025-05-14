// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/AbilitySystem/AttributeSets/ASCharacteristics.h"
#include "Global/AbilitySystem/GASystemComponent.h"
#include "GameplayAbilitiesModule.h"
#include "AbilitySystemGlobals.h"
#include "Global/Libraries/StaticLibrary.h"

void UASCharacteristics::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	FProperty* CharismaProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, Charisma));
	FProperty* ConstitutionProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, Constitution));
	FProperty* DexterityProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, Dexterity));
	FProperty* IntelligenceProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, Intelligence));
	FProperty* StrengthProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, Strength));
	FProperty* WisdomProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, Wisdom));

	NewValue = FMath::Clamp(NewValue, 0.f, NewValue);

	if (Attribute == CharismaProperty)
	{
		OnCharacteristicModified.Broadcast(ECharacteristic::Charisma, Charisma.GetCurrentValue());
	}
	else if (Attribute == ConstitutionProperty)
	{
		OnCharacteristicModified.Broadcast(ECharacteristic::Constitution, Constitution.GetCurrentValue());
	}
	else if (Attribute == DexterityProperty)
	{
		OnCharacteristicModified.Broadcast(ECharacteristic::Dexterity, Dexterity.GetCurrentValue());
	}
	else if (Attribute == IntelligenceProperty)
	{
		OnCharacteristicModified.Broadcast(ECharacteristic::Intelligence, Intelligence.GetCurrentValue());
	}
	else if (Attribute == StrengthProperty)
	{
		OnCharacteristicModified.Broadcast(ECharacteristic::Strength, Strength.GetCurrentValue());
	}
	else if (Attribute == WisdomProperty)
	{
		OnCharacteristicModified.Broadcast(ECharacteristic::Wisdom, Wisdom.GetCurrentValue());
	}

}

bool UASCharacteristics::ShouldInitProperty(bool FirstInit, FProperty* PropertyToInit) const
{
	//// Get all property members here that should be initialized based on other Attributes
	//FProperty* VitalityProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, Vitality));
	//FProperty* VigorProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, Vigor));
	//FProperty* MentalEnergyProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, MentalEnergy));

	//FProperty* ConHealthBonusProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, ConHealthBonus));
	//FProperty* DexHealthBonusProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, DexHealthBonus));
	//FProperty* StrHealthBonusProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, StrHealthBonus));

	//FProperty* ConStaminaBonusProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, ConStaminaBonus));
	//FProperty* DexStaminaBonusProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, DexStaminaBonus));
	//FProperty* StrStaminaBonusProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, StrStaminaBonus));

	//FProperty* IntManaBonusProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, IntManaBonus));
	//FProperty* WisManaBonusProperty = FindFieldChecked<FProperty>(UASCharacteristics::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacteristics, WisManaBonus));

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
