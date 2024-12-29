// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Characters/HealthComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Global/AbilitySystem/AttributeSets/ASHealth.h"
#include "Global/AbilitySystem/AttributeSets/AttributeSetInfo.h"

#define LOCTEXT_NAMESPACE "UHealthComponent"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// ...
	HealthSet = nullptr;

	if (!AttributeInfo)
	{
		const FSoftObjectPath DefaultItemPath(TEXT("/Game/DataRegistries/HealthSetInfo"));
		AttributeInfo = Cast<UAttributeSetInfo>(DefaultItemPath.TryLoad());
	};
}

void UHealthComponent::OnInitialized()
{
	HealthSet = ASC->AddSet<UASHealth>();
	if (!HealthSet)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(GetOwner()));
		return;
	}

	HealthSet->OnHealthModified.AddUObject(this, &ThisClass::HandleAttributeModified);
	HealthSet->OnMaxHealthModified.AddUObject(this, &ThisClass::HandleMaxAttributeModified);
	HealthSet->OnHealthZeroed.AddUObject(this, &ThisClass::HandleAttributeZeroed);

	OnAttributeModified.Broadcast(FOnAttributeModifiedPayload(HealthSet->GetHealth(), HealthSet->GetHealth()));
	OnMaxAttributeModified.Broadcast(FOnAttributeModifiedPayload(HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth()));
}

void UHealthComponent::OnUninitialized()
{
	if (HealthSet)
	{
		HealthSet->OnHealthModified.RemoveAll(this);
		HealthSet->OnMaxHealthModified.RemoveAll(this);
		HealthSet->OnHealthZeroed.RemoveAll(this);
	}

	HealthSet = nullptr;
}

void UHealthComponent::OnModified(const FOnAttributeModifiedPayload& Payload) const
{
	if (HealthSet->GetHealth() == HealthSet->GetMaxHealth())
	{
		AddFullAttributeTag();
	}
	else
	{
		RemoveFullAttributeTag();
	}

	UAttributeComponent::OnModified(Payload);
}

FText UHealthComponent::GetComponentDisplayName() const
{
	return LOCTEXT("HealthComponent", "Health");
}

FText UHealthComponent::GetComponentDescription() const
{
	return LOCTEXT("HealthDescription", "The energy within the body that is used when performing any activities. This is restored by sleep and food.");
}

float UHealthComponent::GetComponentSummaryValue() const
{
	return 0.0f;
}

#undef LOCTEXT_NAMESPACE
