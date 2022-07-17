// Fill out your copyright notice in the Description page of Project Settings.


#include "ASUltimate.h"
#include "GACharacter.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UASUltimate::UASUltimate()
	: MaxUltimateCharge(1200.f)
	, UltimateCharge(0.f)
	, UltimateChargeRate(0.216f)
{
}

//
void UASUltimate::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	FProperty* UltimateProperty = FindFieldChecked<FProperty>(UASUltimate::StaticClass(), GET_MEMBER_NAME_CHECKED(UASUltimate, UltimateCharge));
	FProperty* MaxUltimateProperty = FindFieldChecked<FProperty>(UASUltimate::StaticClass(), GET_MEMBER_NAME_CHECKED(UASUltimate, MaxUltimateCharge));
	FProperty* UltimateChargeRateProperty = FindFieldChecked<FProperty>(UASUltimate::StaticClass(), GET_MEMBER_NAME_CHECKED(UASUltimate, UltimateChargeRate));

	if (Attribute == UltimateProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxUltimateCharge.GetCurrentValue());

		AGACharacter* OwningCharacter = Cast<AGACharacter>(GetOwningActor());

		if (OwningCharacter)
		{
			if (UltimateCharge.GetCurrentValue() == MaxUltimateCharge.GetCurrentValue())
			{
				OwningCharacter->AddGameplayTag(OwningCharacter->FullUltimateTag);
			}
			else
			{
				OwningCharacter->RemoveGameplayTag(OwningCharacter->FullUltimateTag);
			}
		}
	}
	else if (Attribute == MaxUltimateProperty)
	{
		NewValue = FMath::Clamp(NewValue, 1.0f, NewValue);
	}
	else if (Attribute == UltimateChargeRateProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, NewValue);
	}
}

//
bool UASUltimate::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	Super::PreGameplayEffectExecute(Data);

	bool bModifiedUltimate = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASUltimate::StaticClass(), GET_MEMBER_NAME_CHECKED(UASUltimate, UltimateCharge));

	if (bModifiedUltimate)
	{
		bool bDamagedUltimate = Data.EvaluatedData.Magnitude < UltimateCharge.GetCurrentValue();

		if (bDamagedUltimate)
		{

		}
	}

	return true;
}

//
void UASUltimate::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	AGACharacter* OwningCharacter = Cast<AGACharacter>(GetOwningActor());

	if (OwningCharacter)
	{
		OwningCharacter->OnUltimateModified(UltimateCharge.GetCurrentValue(), MaxUltimateCharge.GetCurrentValue());
	}
}
