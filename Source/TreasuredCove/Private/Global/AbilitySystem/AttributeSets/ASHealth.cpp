// Fill out your copyright notice in the Description page of Project Settings.

#include "ASHealth.h"
#include "HealthComponent.h"

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
		OnHealthZeroed.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), CurrentHealth));
	}

	bHealthZeroed = (CurrentHealth <= 0.0f);
}

void UASHealth::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UASHealth, MaxHealth, OldValue);

	OnMaxHealthModified.Broadcast(FOnAttributeModifiedPayload(OldValue.GetCurrentValue(), GetMaxHealth()));
}

UASHealth::UASHealth()
	: MaxHealth(10.f)
	, Health(10.f)
	, MaxStamina(10.f)
	, Stamina(10.f)
	, MaxMana(10.f)
	, Mana(10.f)
	, Defense(0.f)
	, DefenseMultiplier(1.f)
{
}

void UASHealth::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Get all property members here that should be checked after modification
	FProperty* HealthProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Health));
	FProperty* StaminaProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Stamina));
	FProperty* ManaProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Mana));
	FProperty* MaxHealthProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxHealth));
	FProperty* MaxStaminaProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxStamina));
	FProperty* MaxManaProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxMana));

	// Now we need to increment through all property members that could be modified
	if (Attribute == HealthProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
		// UE_LOG(LogTemp, Warning, TEXT("Health Value:%f"), NewValue);

		UHealthComponent* HealthComp = GetOwningActor()->GetComponentByClass<UHealthComponent>();
		if (HealthComp)
		{
			if (Health.GetCurrentValue() == MaxHealth.GetCurrentValue())
			{
				HealthComp->AddFullHealthTag();
			}
			else
			{
				HealthComp->RemoveFullHealthTag();
			}
		}
	}
	else if (Attribute == StaminaProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetCurrentValue());
	}
	else if (Attribute == ManaProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxMana.GetCurrentValue());
	}
	else if (Attribute == MaxHealthProperty)
	{
		NewValue = FMath::Clamp(NewValue, Health.GetCurrentValue(), NewValue);
	}
	else if (Attribute == MaxStaminaProperty)
	{
		NewValue = FMath::Clamp(NewValue, Stamina.GetCurrentValue(), NewValue);
	} 
	else if (Attribute == MaxManaProperty)
	{
		NewValue = FMath::Clamp(NewValue, Mana.GetCurrentValue(), NewValue);
	}
}

bool UASHealth::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
		return false;

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

	if (SourceAbilitySystem)
	{
		AActor* SourceActor = SourceAbilitySystem->GetOwner();

		if (SourceActor)
		{
			// If health has actually changed activate callbacks
			if (GetHealth() != HealthBeforeAttributeChange)
			{
				OnHealthModified.Broadcast(FOnAttributeModifiedPayload(HealthBeforeAttributeChange, GetHealth(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
			}

			if ((GetHealth() <= 0.0f) && !bHealthZeroed)
			{
				OnHealthZeroed.Broadcast(FOnAttributeModifiedPayload(HealthBeforeAttributeChange, GetHealth(), GetOwningActor(), Causer, Instigator, &Data.EffectSpec));
			}

			// Check health again in case an event above changed it.
			bHealthZeroed = (GetHealth() <= 0.0f);
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
