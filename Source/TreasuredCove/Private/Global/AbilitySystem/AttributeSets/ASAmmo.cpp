// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/AbilitySystem/AttributeSets/ASAmmo.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UASAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UASAmmo, Ammo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UASAmmo, MaxAmmo, COND_None, REPNOTIFY_Always);
}

void UASAmmo::OnRep_Ammo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASAmmo, Ammo, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentAmmo = GetAmmo();

	OnAmmoModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentAmmo));

	if (!bAmmoZeroed && CurrentAmmo <= 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(-3, 5, FColor::Blue, FString("Replicated Ammo Zeroed Called"));
		OnAmmoZeroed.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentAmmo));
	}

	bAmmoZeroed = (CurrentAmmo <= 0.0f);
}
void UASAmmo::OnRep_MaxAmmo(const FGameplayAttributeData& OldValue)
{
}

UASAmmo::UASAmmo()
	: MaxAmmo(10.f)
	, Ammo(10.f)
{
}

void UASAmmo::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	FProperty* AmmoProperty = FindFieldChecked<FProperty>(UASAmmo::StaticClass(), GET_MEMBER_NAME_CHECKED(UASAmmo, Ammo));
	
	if (Attribute == AmmoProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxAmmo.GetCurrentValue());
	}
}

bool UASAmmo::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	Super::PreGameplayEffectExecute(Data);

	return true;
}

void UASAmmo::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* SourceAbilitySystem = GetOwningAbilitySystemComponent();
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (SourceAbilitySystem)
	{
		AActor* SourceActor = SourceAbilitySystem->GetOwner();

		if (SourceActor)
		{
			const float CurrentAmmo = GetAmmo();

			// If Ammo has actually changed activate callbacks
			if (CurrentAmmo != AmmoBeforeAttributeChange)
			{
				OnAmmoModified.Broadcast(FOnAttributeModifiedPayload(AmmoBeforeAttributeChange, GetAmmo(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
			}

			if (!bAmmoZeroed && (CurrentAmmo <= 0.0f))
			{
				GEngine->AddOnScreenDebugMessage(-2, 5, FColor::Blue, FString("Ammo Zeroed Called"));
				OnAmmoZeroed.Broadcast(FOnAttributeModifiedPayload(AmmoBeforeAttributeChange, GetAmmo(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
			}

			// Check Ammo again in case an event above changed it.
			bAmmoZeroed = (CurrentAmmo <= 0.0f);
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
