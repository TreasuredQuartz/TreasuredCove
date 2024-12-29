// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/Abilities/GA_SpawnDamageDealtNumbers.h"

UGA_SpawnDamageDealtNumbers::UGA_SpawnDamageDealtNumbers()
{
	FAbilityTriggerData TriggerEvent;
	TriggerEvent.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerEvent.TriggerTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.damaged"));

	AbilityTriggers.Add(TriggerEvent);
}

void UGA_SpawnDamageDealtNumbers::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
}