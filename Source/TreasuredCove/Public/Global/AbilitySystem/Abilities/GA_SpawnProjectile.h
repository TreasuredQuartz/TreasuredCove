// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "GA_SpawnProjectile.generated.h"

class AGAProjectile;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGA_SpawnProjectile : public UGameplayAbilityBase
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
public:
	FVector AbilitySpawnLocation;
	TSubclassOf<AGAProjectile> ProjectileClass;
};
