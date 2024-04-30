// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Characters/HealthComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "ASHealth.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	// ...
	HealthSet = nullptr;
	ASC = nullptr;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		InitializeWithAbilitySystem(ASI->GetAbilitySystemComponent());
	}
}

void UHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void UHealthComponent::InitializeWithAbilitySystem(UAbilitySystemComponent* InASC)
{
	ASC = InASC;
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(GetOwner()));
		return;
	}

	HealthSet = ASC->AddSet<UASHealth>();;
	if (!HealthSet)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(GetOwner()));
		return;
	}

	HealthSet->OnHealthModified.AddUObject(this, &ThisClass::HandleHealthModified);
	HealthSet->OnMaxHealthModified.AddUObject(this, &ThisClass::HandleMaxHealthModified);
	HealthSet->OnHealthZeroed.AddUObject(this, &ThisClass::HandleHealthZeroed);

	OnHealthModified.Broadcast(FOnAttributeModifiedPayload(HealthSet->GetHealth(), HealthSet->GetHealth()));
	OnMaxHealthModified.Broadcast(FOnAttributeModifiedPayload(HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth()));
}

void UHealthComponent::UninitializeFromAbilitySystem()
{
	if (HealthSet)
	{
		HealthSet->OnHealthModified.RemoveAll(this);
		HealthSet->OnMaxHealthModified.RemoveAll(this);
		HealthSet->OnHealthZeroed.RemoveAll(this);
	}

	HealthSet = nullptr;
	ASC = nullptr;
}

void UHealthComponent::AddFullHealthTag() const
{
	ASC->AddLooseGameplayTag(FullHealthTag);
	ASC->SetTagMapCount(FullHealthTag, 1);
}

void UHealthComponent::RemoveFullHealthTag() const
{
	ASC->RemoveLooseGameplayTag(FullHealthTag);
}

void UHealthComponent::HandleHealthModified(const FOnAttributeModifiedPayload& Payload) const
{
	OnHealthModified.Broadcast(Payload);
}

void UHealthComponent::HandleMaxHealthModified(const FOnAttributeModifiedPayload& Payload) const
{
	OnMaxHealthModified.Broadcast(Payload);
}

void UHealthComponent::HandleHealthZeroed(const FOnAttributeModifiedPayload& Payload)
{
	bIsHealthZeroed = true;
	GEngine->AddOnScreenDebugMessage(-2, 5, FColor::Blue, FString("HealthComponent: OnHealthZeroed Called"));
	OnHealthZeroed.Broadcast(Payload.Victim, Payload.Instigator);
}

// Called every frame
//void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

