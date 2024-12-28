// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/AmmoComponent.h"
#include "AbilitySystemComponent.h"
#include "Global/AbilitySystem/AttributeSets/ASAmmo.h"

// Sets default values for this component's properties
UAmmoComponent::UAmmoComponent()
{
	// ...
	AmmoSet = nullptr;
}

void UAmmoComponent::OnInitialized()
{
	AmmoSet = ASC->AddSet<UASAmmo>();
	if (!AmmoSet)
	{
		UE_LOG(LogTemp, Error, TEXT("AmmoComponent: Cannot initialize health component for owner [%s] with NULL ammo set on the ability system."), *GetNameSafe(GetOwner()));
		return;
	}

	AmmoSet->OnAmmoModified.AddUObject(this, &ThisClass::HandleAttributeModified);
	AmmoSet->OnMaxAmmoModified.AddUObject(this, &ThisClass::HandleMaxAttributeModified);
	AmmoSet->OnAmmoZeroed.AddUObject(this, &ThisClass::HandleAttributeZeroed);

	OnAttributeModified.Broadcast(FOnAttributeModifiedPayload(AmmoSet->GetAmmo(), AmmoSet->GetAmmo()));
	OnMaxAttributeModified.Broadcast(FOnAttributeModifiedPayload(AmmoSet->GetMaxAmmo(), AmmoSet->GetMaxAmmo()));
}

void UAmmoComponent::OnUninitialized()
{
	if (AmmoSet)
	{
		AmmoSet->OnAmmoModified.RemoveAll(this);
		AmmoSet->OnMaxAmmoModified.RemoveAll(this);
		AmmoSet->OnAmmoZeroed.RemoveAll(this);
	}

	AmmoSet = nullptr;
}


