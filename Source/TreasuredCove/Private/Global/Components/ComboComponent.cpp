// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/ComboComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UComboComponent::UComboComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UComboComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

/*
// Called every frame
void UComboComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}*/


int UComboComponent::DetermineCombo(const struct FGameplayTag& ComboCooldownTag)
{
	int Value = -1;

	if (!bInCombo)
	{
		// Starting combo
		bInCombo = true;

		GetWorld()->GetTimerManager().SetTimer(EndComboTimerHandle, this, &UComboComponent::EndCombo, EndComboCooldown, false);
	}

	if (ComboCount < MaxComboCount)
	{
		if (GetOwner())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
			{
				ASC->AddLooseGameplayTag(ComboCooldownTag);
				ASC->SetTagMapCount(ComboCooldownTag, 1);
			}
		}

		FTimerDelegate ComboDel;
		ComboDel.BindUFunction(this, "ComboCooldown", ComboCooldownTag);

		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, ComboDel, ComboContinueTime, false);

		Value = ComboCount;
		++ComboCount;
	}

	return Value;
}

// *ENDS* Combo cooldown
void UComboComponent::ComboCooldown(const struct FGameplayTag& CooldownTag)
{
	if (GetOwner())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
			ASC->RemoveLooseGameplayTag(CooldownTag);
	}
}

// Resets combo values
void UComboComponent::EndCombo()
{
	bInCombo = false;
	ComboCount = 0;
}
