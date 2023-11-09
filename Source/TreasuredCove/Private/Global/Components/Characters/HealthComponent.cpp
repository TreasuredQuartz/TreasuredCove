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
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	HealthSet = CreateDefaultSubobject<UASHealth>(TEXT("Health Attribute Set"));
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		ASC = ASI->GetAbilitySystemComponent();
	}

	if (ASC)
	{
		ASC->AddAttributeSetSubobject(HealthSet);
	}
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

void UHealthComponent::DamageHealth(const FOnHealthDamagedResult& Result) const
{
	OnHealthDamaged.Broadcast(Result);
}

void UHealthComponent::Die(const AActor* Victim, const AActor* ResponsibleActor) const
{
	OnDeath.Broadcast(Victim, ResponsibleActor);
}


// Called every frame
//void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

