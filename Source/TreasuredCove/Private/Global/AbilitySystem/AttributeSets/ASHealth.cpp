// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/AbilitySystem/AttributeSets/ASHealth.h"
#include "Global/Components/Characters/HealthComponent.h"

#include "AbilitySystemComponent.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

void UASHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UASHealth, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASHealth, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASHealth, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASHealth, HealthRegenDelay, COND_None, REPNOTIFY_Always);
}

void UASHealth::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASHealth, Health, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentHealth = GetHealth();

	OnHealthModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentHealth));

	if (!bHealthZeroed && CurrentHealth <= 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-3, 5, FColor::Blue, FString("Replicated Health Zeroed Called"));
		OnHealthZeroed.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentHealth));
	}

	bHealthZeroed = (CurrentHealth <= 0.0f);
}

void UASHealth::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASHealth, MaxHealth, OldValue);

	OnMaxHealthModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), GetMaxHealth()));
}

void UASHealth::OnRep_HealthRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASHealth, HealthRegenRate, OldValue);

	OnHealthRegenRateModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), GetHealthRegenRate()));
}

void UASHealth::OnRep_HealthRegenDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASHealth, HealthRegenDelay, OldValue);

	OnHealthRegenDelayModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), GetHealthRegenDelay()));
}

UASHealth::UASHealth()
	: bHealthZeroed(false)
	, MaxHealth(10.f)
	, Health(10.f)
	, Defense(0.f)
	, DefenseMultiplier(1.f)
{
}

void UASHealth::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Get all property members here that should be checked after modification
	FProperty* HealthProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Health));
	FProperty* MaxHealthProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxHealth));
	FProperty* HealthRegenRateProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, HealthRegenRate));
	FProperty* HealthRegenDelayProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, HealthRegenDelay));

	// Now we need to increment through all property members that could be modified
	if (Attribute == HealthProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealth.GetCurrentValue());
	}
	else if (Attribute == MaxHealthProperty)
	{
		NewValue = FMath::Clamp(NewValue, Health.GetCurrentValue(), NewValue);
	}
	else if (Attribute == HealthRegenRateProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0, NewValue);
	}
	else if (Attribute == HealthRegenDelayProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0, NewValue);
	}
}

bool UASHealth::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	if (!Super::PreGameplayEffectExecute(Data)) return false;

	HealthBeforeAttributeChange = GetHealth();

	return true;
}

void UASHealth::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* SourceAbilitySystem = GetOwningAbilitySystemComponent();
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(GetHealth()));

	if (SourceAbilitySystem)
	{
		const float CurrentHealth = GetHealth();

		// If health has actually changed activate callbacks
		if (CurrentHealth != HealthBeforeAttributeChange)
		{
			OnHealthModified.Broadcast(FOnAttributeModifiedPayload(HealthBeforeAttributeChange, GetHealth(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
		}

		if (!bHealthZeroed && (CurrentHealth <= 0.0f))
		{
			GEngine->AddOnScreenDebugMessage(-2, 5, FColor::Blue, FString("Health Zeroed Called"));
			OnHealthZeroed.Broadcast(FOnAttributeModifiedPayload(HealthBeforeAttributeChange, GetHealth(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
		}

		// Check health again in case an event above changed it.
		bHealthZeroed = (CurrentHealth <= 0.0f);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Source AbilitySystem is null");
	}
}
