// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/AbilitySystem/AttributeSets/ASStamina.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

void UASStamina::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UASStamina, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASStamina, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASStamina, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASStamina, StaminaRegenDelay, COND_None, REPNOTIFY_Always);
}

void UASStamina::OnRep_Stamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASStamina, Stamina, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentStamina = GetStamina();

	OnStaminaModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentStamina));

	if (!bStaminaZeroed && CurrentStamina <= 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-3, 5, FColor::Blue, FString("Replicated Stamina Zeroed Called"));
		OnStaminaZeroed.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentStamina));
	}

	bStaminaZeroed = (CurrentStamina <= 0.0f);
}

void UASStamina::OnRep_MaxStamina(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASStamina, MaxStamina, OldValue);

	OnMaxStaminaModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), GetMaxStamina()));
}

void UASStamina::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASStamina, StaminaRegenRate, OldValue);
}

void UASStamina::OnRep_StaminaRegenDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASStamina, StaminaRegenDelay, OldValue);
}

UASStamina::UASStamina()
{
}

void UASStamina::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Get all property members here that should be checked after modification
	FProperty* StaminaProperty = FindFieldChecked<FProperty>(UASStamina::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStamina, Stamina));
	FProperty* MaxStaminaProperty = FindFieldChecked<FProperty>(UASStamina::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStamina, MaxStamina));
	FProperty* StaminaRegenRateProperty = FindFieldChecked<FProperty>(UASStamina::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStamina, StaminaRegenRate));
	FProperty* StaminaRegenDelayProperty = FindFieldChecked<FProperty>(UASStamina::StaticClass(), GET_MEMBER_NAME_CHECKED(UASStamina, StaminaRegenDelay));

	// Now we need to increment through all property members that could be modified
	if (Attribute == StaminaProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxStamina.GetCurrentValue());
	}
	else if (Attribute == MaxStaminaProperty)
	{
		NewValue = FMath::Clamp(NewValue, Stamina.GetCurrentValue(), NewValue);
	}
	else if (Attribute == StaminaRegenRateProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0, NewValue);
	}
	else if (Attribute == StaminaRegenDelayProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0, NewValue);
	}
}

bool UASStamina::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data)) return false;

	StaminaBeforeAttributeChange = GetStamina();

	return true;
}

void UASStamina::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* SourceAbilitySystem = GetOwningAbilitySystemComponent();
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(GetStamina()));

	if (SourceAbilitySystem)
	{
		AActor* SourceActor = SourceAbilitySystem->GetOwner();

		if (SourceActor)
		{
			const float CurrentStamina = GetStamina();

			// If Stamina has actually changed activate callbacks
			if (CurrentStamina != StaminaBeforeAttributeChange)
			{
				OnStaminaModified.Broadcast(FOnAttributeModifiedPayload(StaminaBeforeAttributeChange, GetStamina(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
			}

			if (!bStaminaZeroed && (CurrentStamina <= 0.0f))
			{
				GEngine->AddOnScreenDebugMessage(-2, 5, FColor::Blue, FString("Stamina Zeroed Called"));
				OnStaminaZeroed.Broadcast(FOnAttributeModifiedPayload(StaminaBeforeAttributeChange, GetStamina(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
			}

			// Check Stamina again in case an event above changed it.
			bStaminaZeroed = (CurrentStamina <= 0.0f);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "SourceActor is null");
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Source AbilitySystem is null");
	}
}
