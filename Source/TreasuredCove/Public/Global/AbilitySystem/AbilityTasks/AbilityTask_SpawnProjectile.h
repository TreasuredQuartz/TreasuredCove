// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_SpawnProjectile.generated.h"

class AGAProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnProjectileDelegate, AGAProjectile*, SpawnedActor);

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UAbilityTask_SpawnProjectile : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSpawnProjectileDelegate OnSpawned;

	UPROPERTY(BlueprintAssignable)
	FSpawnProjectileDelegate DidNotSpawn;

	UPROPERTY(BlueprintAssignable)
	FSpawnProjectileDelegate Event;

	UFUNCTION(BlueprintCallable, meta = (
		HidePin = "OwningAbility",
		DefaultToSelf = "OwningAbility",
		BlueprintInternalUseOnly = "true"
		), Category = "Ability|Tasks")
		static UAbilityTask_SpawnProjectile* SpawnProjectile(UGameplayAbility* OwningAbility, TSubclassOf<AGAProjectile> InClass, FTransform InLocation, float InProjectileVelocity, FGameplayEffectSpecHandle InGESpecHandle);

	// virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	bool BeginSpawningActor(UGameplayAbility* OwningAbility, TSubclassOf<AGAProjectile> InClass, FTransform InLocation, float InProjectileVelocity, AGAProjectile*& SpawnedActor, FGameplayEffectSpecHandle InGESpecHandle);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	void FinishSpawningActor(UGameplayAbility* OwningAbility, TSubclassOf<AGAProjectile> InClass, FTransform InLocation, float InProjectileVelocity, AGAProjectile* SpawnedActor, FGameplayEffectSpecHandle InGESpecHandle);


public:
	FTransform Location;
	float ProjectileVelocity;
	FGameplayEffectSpecHandle GESpecHandle;
};
