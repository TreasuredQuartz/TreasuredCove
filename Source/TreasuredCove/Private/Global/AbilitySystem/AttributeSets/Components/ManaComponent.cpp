// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/AbilitySystem/AttributeSets/Components/ManaComponent.h"
#include "AbilitySystemComponent.h"
#include "Global/AbilitySystem/AttributeSets/AttributeSetInfo.h"
#include "Global/AbilitySystem/AttributeSets/ASMana.h"

#define LOCTEXT_NAMESPACE "UManaComponent"

UManaComponent::UManaComponent()
{
	// ...
	ManaSet = nullptr;

	if (!AttributeInfo)
	{
		const FSoftObjectPath DefaultItemPath(TEXT("/Game/DataRegistries/ManaSetInfo"));
		AttributeInfo = Cast<UAttributeSetInfo>(DefaultItemPath.TryLoad());
	};
}

void UManaComponent::OnInitialized()
{
	ManaSet = ASC->AddSet<UASMana>();
	if (!ManaSet)
	{
		UE_LOG(LogTemp, Error, TEXT("ManaComponent: Cannot initialize Mana component for owner [%s] with NULL Mana set on the ability system."), *GetNameSafe(GetOwner()));
		return;
	}

	ManaSet->OnManaModified.AddUObject(this, &ThisClass::HandleAttributeModified);
	ManaSet->OnMaxManaModified.AddUObject(this, &ThisClass::HandleMaxAttributeModified);
	ManaSet->OnManaZeroed.AddUObject(this, &ThisClass::HandleAttributeZeroed);

	OnAttributeModified.Broadcast(FOnAttributeModifiedPayload(ManaSet->GetMana(), ManaSet->GetMana()));
	OnMaxAttributeModified.Broadcast(FOnAttributeModifiedPayload(ManaSet->GetMaxMana(), ManaSet->GetMaxMana()));
}

void UManaComponent::OnUninitialized()
{
	if (ManaSet)
	{
		ManaSet->OnManaModified.RemoveAll(this);
		ManaSet->OnMaxManaModified.RemoveAll(this);
		ManaSet->OnManaZeroed.RemoveAll(this);
	}

	ManaSet = nullptr;
}

void UManaComponent::OnModified(const FOnAttributeModifiedPayload& Payload) const
{
	if (ManaSet->GetMana() == ManaSet->GetMaxMana())
	{
		AddFullAttributeTag();
	}
	else
	{
		RemoveFullAttributeTag();
	}

	UAttributeComponent::OnModified(Payload);
}

FText UManaComponent::GetComponentDisplayName() const
{
	return LOCTEXT("ManaComponent", "Mana");
}

FText UManaComponent::GetComponentDescription() const
{
	return LOCTEXT("ManaDescription", "The energy within the body that is used when performing any activities. This is restored by sleep and food.");
}

float UManaComponent::GetComponentSummaryValue() const
{
	return 0.0f;
}

#undef LOCTEXT_NAMESPACE