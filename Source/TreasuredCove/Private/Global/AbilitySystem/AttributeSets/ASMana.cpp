// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/AbilitySystem/AttributeSets/ASMana.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

void UASMana::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UASMana, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASMana, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASMana, ManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASMana, ManaRegenDelay, COND_None, REPNOTIFY_Always);
}

void UASMana::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASMana, Mana, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentMana = GetMana();

	OnManaModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentMana));

	if (!bManaZeroed && CurrentMana <= 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-3, 5, FColor::Blue, FString("Replicated Mana Zeroed Called"));
		OnManaZeroed.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentMana));
	}

	bManaZeroed = (CurrentMana <= 0.0f);
}

void UASMana::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASMana, MaxMana, OldValue);
}

void UASMana::OnRep_ManaRegenRate(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASMana, ManaRegenRate, OldValue);
}

void UASMana::OnRep_ManaRegenDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASMana, ManaRegenDelay, OldValue);
}

UASMana::UASMana()
{
}

void UASMana::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Get all property members here that should be checked after modification
	FProperty* ManaProperty = FindFieldChecked<FProperty>(UASMana::StaticClass(), GET_MEMBER_NAME_CHECKED(UASMana, Mana));
	FProperty* MaxManaProperty = FindFieldChecked<FProperty>(UASMana::StaticClass(), GET_MEMBER_NAME_CHECKED(UASMana, MaxMana));
	FProperty* ManaRegenRateProperty = FindFieldChecked<FProperty>(UASMana::StaticClass(), GET_MEMBER_NAME_CHECKED(UASMana, ManaRegenRate));
	FProperty* ManaRegenDelayProperty = FindFieldChecked<FProperty>(UASMana::StaticClass(), GET_MEMBER_NAME_CHECKED(UASMana, ManaRegenDelay));

	// Now we need to increment through all property members that could be modified
	if (Attribute == ManaProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxMana.GetCurrentValue());
	}
	else if (Attribute == MaxManaProperty)
	{
		NewValue = FMath::Clamp(NewValue, Mana.GetCurrentValue(), NewValue);
	}
	else if (Attribute == ManaRegenRateProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0, NewValue);
	}
	else if (Attribute == ManaRegenDelayProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0, NewValue);
	}
}

bool UASMana::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data)) return false;

	ManaBeforeAttributeChange = GetMana();

	return true;
}

void UASMana::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* SourceAbilitySystem = GetOwningAbilitySystemComponent();
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::SanitizeFloat(GetMana()));

	if (SourceAbilitySystem)
	{
		AActor* SourceActor = SourceAbilitySystem->GetOwner();

		if (SourceActor)
		{
			const float CurrentMana = GetMana();

			// If Mana has actually changed activate callbacks
			if (CurrentMana != ManaBeforeAttributeChange)
			{
				OnManaModified.Broadcast(FOnAttributeModifiedPayload(ManaBeforeAttributeChange, GetMana(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
			}

			if (!bManaZeroed && (CurrentMana <= 0.0f))
			{
				GEngine->AddOnScreenDebugMessage(-2, 5, FColor::Blue, FString("Mana Zeroed Called"));
				OnManaZeroed.Broadcast(FOnAttributeModifiedPayload(ManaBeforeAttributeChange, GetMana(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
			}

			// Check Mana again in case an event above changed it.
			bManaZeroed = (CurrentMana <= 0.0f);
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

