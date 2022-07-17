// Fill out your copyright notice in the Description page of Project Settings.

#include "ASWeaponStats.h"
#include "GACharacter.h"

#include "AbilitySystemComponent.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UASWeaponStats::UASWeaponStats()
	: SpreadAngle(0)
	, SpreadMin(0)
	, SpreadMax(1)
	, SpreadDecreaseSpeed(0.1)
	, SpreadIncreaseSpeed(0.1)
	, SpreadDeltaMultiplier(1)
	, Sway(1)
	, HorizontalRecoil(1)
	, VerticalRecoil(1)
	, ReloadSpeed(1)
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

	FProperty* SpreadAngleProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadAngle));
	FProperty* SpreadMinProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadMin));
	FProperty* SpreadMaxProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadMax));
	FProperty* SpreadDecreaseSpeedProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadDecreaseSpeed));
	FProperty* SpreadIncreaseSpeedProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadIncreaseSpeed));
	FProperty* SpreadDeltaMultiplierProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadDeltaMultiplier));
	FProperty* SwayProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, Sway));
	FProperty* HorizontalRecoilProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, HorizontalRecoil));
	FProperty* VerticalRecoilProperty = FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, VerticalRecoil));
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

	if (Attribute == SpreadAngleProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.f);
	}
	else if (Attribute == SpreadMinProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.0f);
	}
	else if (Attribute == SpreadMaxProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.0f);
	}
	else if (Attribute == SpreadDecreaseSpeedProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == SpreadIncreaseSpeedProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, NewValue);
	}
	else if (Attribute == SpreadDeltaMultiplierProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.01f, NewValue);
	}
	else if (Attribute == SwayProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.0f);
	}
	else if (Attribute == HorizontalRecoilProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.0f);
	}
	else if (Attribute == VerticalRecoilProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 1.0f);
	}
	else if (Attribute == ReloadSpeedProperty)
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

	bool bIsSpreadAngle = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadAngle));
	bool bIsSpreadMin = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadMin));
	bool bIsSpreadMax = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadMax));
	bool bIsSpreadDecreaseSpeed = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadDecreaseSpeed));
	bool bIsSpreadIncreaseSpeed = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadIncreaseSpeed));
	bool bIsSpreadDeltaMultiplier = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, SpreadDeltaMultiplier));
	bool bIsSway = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, Sway));
	bool bIsHorizontalRecoil = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, HorizontalRecoil));
	bool bIsVerticalRecoil = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASWeaponStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASWeaponStats, VerticalRecoil));
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

	if (bIsSpreadAngle)
	{
		OnSpreadAngleModified.Broadcast(SpreadAngle.GetCurrentValue());
	}
	else if (bIsSpreadMin || bIsSpreadMax || bIsSpreadDecreaseSpeed || bIsSpreadIncreaseSpeed || bIsSpreadDeltaMultiplier)
	{
		OnHipAccuracyModified.Broadcast(SpreadMin.GetCurrentValue(), 
			SpreadMax.GetCurrentValue(), 
			SpreadDecreaseSpeed.GetCurrentValue(), 
			SpreadIncreaseSpeed.GetCurrentValue(), 
			SpreadDeltaMultiplier.GetCurrentValue());
	}
	else if (bIsSway || bIsHorizontalRecoil || bIsVerticalRecoil)
	{
		OnADSAccuracyModified.Broadcast(Sway.GetCurrentValue(), 
			HorizontalRecoil.GetCurrentValue(), 
			VerticalRecoil.GetCurrentValue());
	}
	else if (bIsReloadSpeed || bIsDrawSpeed || bIsADSSpeed)
	{
		OnHandlingModified.Broadcast(ReloadSpeed.GetCurrentValue(),
			DrawSpeed.GetCurrentValue(),
			ADSSpeed.GetCurrentValue());
	}
	else if (bIsMinRange || bIsMaxRange)
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