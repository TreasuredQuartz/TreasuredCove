// Fill out your copyright notice in the Description page of Project Settings.

#include "ASAmmo.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

#include "GACharacter.h"

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

		AGACharacter* CharacterOwner = Cast<AGACharacter>(GetOwningActor());
		if (CharacterOwner)
		{
			if (Ammo.GetCurrentValue() == MaxAmmo.GetCurrentValue())
			{
				CharacterOwner->AddGameplayTag(CharacterOwner->FullAmmoTag);
			}
			else
			{
				CharacterOwner->RemoveGameplayTag(CharacterOwner->FullAmmoTag);
			}
		}
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

	OnAmmoModified.Broadcast(Ammo.GetCurrentValue(), MaxAmmo.GetCurrentValue());
	AGACharacter* SourceCharacter = Cast<AGACharacter>(GetOwningActor());

	if (SourceCharacter)
	{
		SourceCharacter->OnAmmoModified(Ammo.GetCurrentValue(), MaxAmmo.GetCurrentValue());
	}
}
