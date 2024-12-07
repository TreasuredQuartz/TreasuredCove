// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/StaminaComponent.h"
#include "AbilitySystemComponent.h"
#include "ASStamina.h"
#include "AttributeSetInfo.h"

#define LOCTEXT_NAMESPACE "UStaminaComponent"

UStaminaComponent::UStaminaComponent()
{
	// ...
	StaminaSet = nullptr;

	if (!AttributeInfo)
	{
		const FSoftObjectPath DefaultItemPath(TEXT("/Game/DataRegistries/StaminaSetInfo"));
		AttributeInfo = Cast<UAttributeSetInfo>(DefaultItemPath.TryLoad());
	};
}

void UStaminaComponent::OnInitialized()
{
	StaminaSet = ASC->AddSet<UASStamina>();
	if (!StaminaSet)
	{
		UE_LOG(LogTemp, Error, TEXT("StaminaComponent: Cannot initialize Stamina component for owner [%s] with NULL Stamina set on the ability system."), *GetNameSafe(GetOwner()));
		return;
	}

	StaminaSet->OnStaminaModified.AddUObject(this, &ThisClass::HandleAttributeModified);
	StaminaSet->OnMaxStaminaModified.AddUObject(this, &ThisClass::HandleMaxAttributeModified);
	StaminaSet->OnStaminaZeroed.AddUObject(this, &ThisClass::HandleAttributeZeroed);

	OnAttributeModified.Broadcast(FOnAttributeModifiedPayload(StaminaSet->GetStamina(), StaminaSet->GetStamina()));
	OnMaxAttributeModified.Broadcast(FOnAttributeModifiedPayload(StaminaSet->GetMaxStamina(), StaminaSet->GetMaxStamina()));
}

void UStaminaComponent::OnUninitialized()
{
	if (StaminaSet)
	{
		StaminaSet->OnStaminaModified.RemoveAll(this);
		StaminaSet->OnMaxStaminaModified.RemoveAll(this);
		StaminaSet->OnStaminaZeroed.RemoveAll(this);
	}

	StaminaSet = nullptr;
}

void UStaminaComponent::OnModified(const FOnAttributeModifiedPayload& Payload) const
{
	if (StaminaSet->GetStamina() == StaminaSet->GetMaxStamina())
	{
		AddFullAttributeTag();
	}
	else
	{
		RemoveFullAttributeTag();
	}

	UAttributeComponent::HandleAttributeModified(Payload);
}

FText UStaminaComponent::GetComponentDisplayName() const
{
	return LOCTEXT("StaminaComponent", "Stamina");
}

FText UStaminaComponent::GetComponentDescription() const
{
	return LOCTEXT("StaminaDescription", "The energy within the body that is used when performing any activities. This is restored by sleep and food.");
}

float UStaminaComponent::GetComponentSummaryValue() const
{
	return 0.0f;
}

#undef LOCTEXT_NAMESPACE
