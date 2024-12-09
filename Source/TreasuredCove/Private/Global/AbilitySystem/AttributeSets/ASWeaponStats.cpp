// Fill out your copyright notice in the Description page of Project Settings.

#include "ASWeaponStats.h"
#include "GACharacter.h"

#include "AbilitySystemComponent.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UASWeaponStats::UASWeaponStats()
	: ReloadSpeed(1)
	, DrawSpeed(1)
	, ADSSpeed(0.25)
	, MinRange(5)
	, MaxRange(100)
	, MinDamage(1)
	, MaxDamage(2)
	, Cooldown(0.25)
	, Cost(1)
	, CastTime(0)
{
}

void UASWeaponStats::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	FProperty* ReloadSpeedProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, ReloadSpeed));
	FProperty* DrawSpeedProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, DrawSpeed));
	FProperty* ADSSpeedProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, ADSSpeed));
	FProperty* MinRangeProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, MinRange));
	FProperty* MaxRangeProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, MaxRange));
	FProperty* MinDamageProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, MinDamage));
	FProperty* MaxDamageProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, MaxDamage));
	FProperty* CooldownProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, Cooldown));
	FProperty* CostProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, Cost));
	FProperty* CastTimeProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, CastTime));

	if (Attribute == ReloadSpeedProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == DrawSpeedProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == ADSSpeedProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == MinRangeProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == MaxRangeProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == MinDamageProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == MaxDamageProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == CooldownProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.01f, NewValue);
	}
	else if (Attribute == CostProperty)
	{
		NewValue = FMath::Clamp(NewValue, 1.f, NewValue);
	}
	else if (Attribute == CastTimeProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}

}

bool UASWeaponStats::ShouldInitProperty(bool FirstInit, FProperty* PropertyToInit) const
{
	Super::ShouldInitProperty(FirstInit, PropertyToInit);
	/*
	FProperty* AttackProperty = FindFieldChecked<FProperty>(UASCombat::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCombat, Attack));

	if (FirstInit)
	{
		return PropertyToInit != AttackProperty;
	}

	return true;
	*/
	return true;
}

void UASWeaponStats::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	bool bIsReloadSpeed = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, ReloadSpeed));
	bool bIsDrawSpeed = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, DrawSpeed));
	bool bIsADSSpeed = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, ADSSpeed));
	bool bIsMinRange = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, MinRange));
	bool bIsMaxRange = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, MaxRange));
	bool bIsMinDamage = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, MinDamage));
	bool bIsMaxDamage = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, MaxDamage));
	bool bIsCooldown = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, Cooldown));
	bool bIsCost = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, Cost));
	bool bIsCastTime = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, CastTime));

	
	if (bIsMinRange || bIsMaxRange)
	{
		OnRangeModified.Broadcast(MinRange.GetCurrentValue(),
		MaxRange.GetCurrentValue());
	}
	else if (bIsMinDamage || bIsMaxDamage)
	{
		OnDamageModified.Broadcast(MinDamage.GetCurrentValue(),
			MaxDamage.GetCurrentValue());
	}
	else if (bIsCooldown || bIsCost || bIsCastTime)
	{
		OnCoreAbilitiesModified.Broadcast(Cooldown.GetCurrentValue(),
			Cost.GetCurrentValue(),
			CastTime.GetCurrentValue());
	}
}