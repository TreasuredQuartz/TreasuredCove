// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/AbilitySystem/AttributeSets/ASGunAccuracy.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UASGunAccuracy::UASGunAccuracy()
	: SpreadAngle(0)
	, SpreadMin(0)
	, SpreadMax(1)
	, SpreadDecreaseSpeed(0.1)
	, SpreadIncreaseSpeed(0.1)
	, SpreadDeltaMultiplier(1)
	, Sway(1)
	, HorizontalRecoil(1)
	, VerticalRecoil(1)
{

}

void UASGunAccuracy::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	FProperty* SpreadAngleProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadAngle));
	FProperty* SpreadMinProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadMin));
	FProperty* SpreadMaxProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadMax));
	FProperty* SpreadDecreaseSpeedProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadDecreaseSpeed));
	FProperty* SpreadIncreaseSpeedProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadIncreaseSpeed));
	FProperty* SpreadDeltaMultiplierProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadDeltaMultiplier));
	FProperty* SwayProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, Sway));
	FProperty* HorizontalRecoilProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, HorizontalRecoil));
	FProperty* VerticalRecoilProperty = FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, VerticalRecoil));

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
}

void UASGunAccuracy::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	bool bIsSpreadAngle = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadAngle));
	bool bIsSpreadMin = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadMin));
	bool bIsSpreadMax = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadMax));
	bool bIsSpreadDecreaseSpeed = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadDecreaseSpeed));
	bool bIsSpreadIncreaseSpeed = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadIncreaseSpeed));
	bool bIsSpreadDeltaMultiplier = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, SpreadDeltaMultiplier));
	bool bIsSway = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, Sway));
	bool bIsHorizontalRecoil = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, HorizontalRecoil));
	bool bIsVerticalRecoil = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASGunAccuracy::StaticClass(), GET_MEMBER_NAME_CHECKED(UASGunAccuracy, VerticalRecoil));

	if (bIsSpreadAngle)
	{
		OnSpreadAngleModified.Broadcast(SpreadAngle.GetCurrentValue());
	}
	/* else if (bIsSpreadMin || bIsSpreadMax || bIsSpreadDecreaseSpeed || bIsSpreadIncreaseSpeed || bIsSpreadDeltaMultiplier)
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
	} */
}