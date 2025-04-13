// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/AbilitySystem/AttributeSets/Components/AttributeComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	// ...
	ASC = nullptr;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		InitializeWithAbilitySystem(ASI->GetAbilitySystemComponent());
	}
}

void UAttributeComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void UAttributeComponent::InitializeWithAbilitySystem(UAbilitySystemComponent* InASC)
{
	ASC = InASC;
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent: Cannot initialize attribute component for owner [%s] with NULL ability system."), *GetNameSafe(GetOwner()));
		return;
	}

	OnInitialized();
}

void UAttributeComponent::UninitializeFromAbilitySystem()
{
	ASC = nullptr;

	OnUninitialized();
}

void UAttributeComponent::AddFullAttributeTag() const
{
	ASC->AddLooseGameplayTag(FullAttributeTag);
	ASC->SetTagMapCount(FullAttributeTag, 1);
}

void UAttributeComponent::RemoveFullAttributeTag() const
{
	ASC->RemoveLooseGameplayTag(FullAttributeTag);
}

void UAttributeComponent::HandleAttributeModified(const FOnAttributeModifiedPayload& Payload) const
{
	OnModified(Payload);

	if (OnAttributeModified.IsBound()) OnAttributeModified.Broadcast(Payload);
}

void UAttributeComponent::HandleMaxAttributeModified(const FOnAttributeModifiedPayload& Payload) const
{
	if (OnMaxAttributeModified.IsBound()) OnMaxAttributeModified.Broadcast(Payload);
}

void UAttributeComponent::HandleAttributeZeroed(const FOnAttributeModifiedPayload& Payload)
{
	bIsZeroed = true;
	if (OnAttributeZeroed.IsBound()) OnAttributeZeroed.Broadcast(Payload.Victim, Payload.Instigator);
}


// Called every frame
//void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

