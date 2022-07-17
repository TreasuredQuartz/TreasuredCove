// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/AbilitySet.h"
#include "AbilitySystemComponent.h"

TArray<TSubclassOf<UGameplayAbility>> UAbilitySet::GetAbilities() const
{
	return Abilities;
}
