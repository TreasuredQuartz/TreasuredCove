// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/AttributeSets/ASUltimate.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UASUltimate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UASUltimate, UltimateCharge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASUltimate, MaxUltimateCharge, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASUltimate, UltimateChargeRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASUltimate, UltimateChargeRegenDelay, COND_None, REPNOTIFY_Always);
}

void UASUltimate::OnRep_UltimateCharge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASUltimate, UltimateCharge, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentUltimateCharge = GetUltimateCharge();

	OnUltimateChargeModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentUltimateCharge));

	if (!bUltimateChargeZeroed && CurrentUltimateCharge <= 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-3, 5, FColor::Blue, FString("Replicated UltimateCharge Zeroed Called"));
		OnUltimateChargeZeroed.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentUltimateCharge));
	}

	bUltimateChargeZeroed = (CurrentUltimateCharge <= 0.0f);
}

void UASUltimate::OnRep_MaxUltimateCharge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASUltimate, MaxUltimateCharge, OldValue);

	OnMaxUltimateChargeModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), GetMaxUltimateCharge()));
}

void UASUltimate::OnRep_UltimateChargeRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASUltimate, UltimateChargeRegenRate, OldValue);

	OnUltimateChargeRegenRateModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), GetUltimateChargeRegenRate()));
}

void UASUltimate::OnRep_UltimateChargeRegenDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASUltimate, UltimateChargeRegenDelay, OldValue);

	OnUltimateChargeRegenDelayModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), GetUltimateChargeRegenDelay()));
}

UASUltimate::UASUltimate()
	: bUltimateChargeZeroed(true)
	, MaxUltimateCharge(1200.f)
	, UltimateCharge(0.f)
	, UltimateChargeRegenRate(0.216f)
{
}

//
void UASUltimate::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	FProperty* UltimateProperty = FindFieldChecked<FProperty>(UASUltimate::StaticClass(), GET_MEMBER_NAME_CHECKED(UASUltimate, UltimateCharge));
	FProperty* MaxUltimateProperty = FindFieldChecked<FProperty>(UASUltimate::StaticClass(), GET_MEMBER_NAME_CHECKED(UASUltimate, MaxUltimateCharge));
	FProperty* UltimateChargeRateProperty = FindFieldChecked<FProperty>(UASUltimate::StaticClass(), GET_MEMBER_NAME_CHECKED(UASUltimate, UltimateChargeRegenRate));

	if (Attribute == UltimateProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxUltimateCharge.GetCurrentValue());
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

	UAbilitySystemComponent* SourceAbilitySystem = GetOwningAbilitySystemComponent();
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	const float CurrentUltimateCharge = GetUltimateCharge();
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(CurrentUltimateCharge));

	if (SourceAbilitySystem)
	{
		// If UltimateCharge has actually changed activate callbacks
		if (CurrentUltimateCharge != UltimateChargeBeforeAttributeChange)
		{
			OnUltimateChargeModified.Broadcast(FOnAttributeModifiedPayload(UltimateChargeBeforeAttributeChange, GetUltimateCharge(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
		}

		if (!bUltimateChargeZeroed && (CurrentUltimateCharge <= 0.0f))
		{
			GEngine->AddOnScreenDebugMessage(-2, 5, FColor::Blue, FString("UltimateCharge Zeroed Called"));
			OnUltimateChargeZeroed.Broadcast(FOnAttributeModifiedPayload(UltimateChargeBeforeAttributeChange, GetUltimateCharge(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
		}

		// Check UltimateCharge again in case an event above changed it.
		bUltimateChargeZeroed = (CurrentUltimateCharge <= 0.0f);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Source AbilitySystem is null");
	}
}
