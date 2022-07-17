// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "GA_SpawnDamageDealtNumbers.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGA_SpawnDamageDealtNumbers : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UGA_SpawnDamageDealtNumbers();
public:
	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
