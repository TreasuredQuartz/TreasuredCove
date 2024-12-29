// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/AbilitySystem/AbilityTasks/AbilityTask_SpawnProjectile.h"
#include "Global/Actors/Items/GAProjectile.h"
#include "Engine/Engine.h"
#include "AbilitySystemComponent.h"
#include "Global/Components/Items/GAProjectileMovementComponent.h"

UAbilityTask_SpawnProjectile* UAbilityTask_SpawnProjectile::SpawnProjectile(UGameplayAbility* OwningAbility, TSubclassOf<AGAProjectile> InClass, FTransform InLocation, float InProjectileVelocity, FGameplayEffectSpecHandle InGESpecHandle)
{
	UAbilityTask_SpawnProjectile* MyObj = NewAbilityTask<UAbilityTask_SpawnProjectile>(OwningAbility);

	FTransform Location = MoveTemp(InLocation);
	float ProjectileVelocity = MoveTemp(InProjectileVelocity);
	FGameplayEffectSpecHandle GESpecHandle = MoveTemp(InGESpecHandle);
	return MyObj;
}

/*
void UAbilityTask_SpawnProjectile::Activate()
{
	bool bSpawnedProjectile = false;

	if (Ability == nullptr)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		AGAProjectile* Proj;
		BeginSpawningActor(ProjectileClass, Location, ProjectileVelocity, Proj, GESpecHandle);

		if (Proj)
		{
			FinishSpawningActor(ProjectileClass, Location, ProjectileVelocity, Proj, GESpecHandle);
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_SpawnProjectile called on invalid AbilitySystemComponent"));
	}
	
}
*/

bool UAbilityTask_SpawnProjectile::BeginSpawningActor(UGameplayAbility* OwningAbility, TSubclassOf<AGAProjectile> InClass, FTransform InLocation, float InProjectileVelocity, AGAProjectile*& SpawnedActor, FGameplayEffectSpecHandle InGESpecHandle)
{
	if (Ability && Ability->GetCurrentActorInfo()->IsNetAuthority() && ShouldBroadcastAbilityTaskDelegates())
	{
		UWorld* const World = GEngine->GetWorldFromContextObject(Ability, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			SpawnedActor = World->SpawnActorDeferred<AGAProjectile>(InClass, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		}
	}

	if (SpawnedActor == nullptr)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(nullptr);
		}
		return false;
	}

	return true;
}

void UAbilityTask_SpawnProjectile::FinishSpawningActor(UGameplayAbility* OwningAbility, TSubclassOf<AGAProjectile> InClass, FTransform InLocation, float InProjectileVelocity, AGAProjectile* SpawnedActor, FGameplayEffectSpecHandle InGESpecHandle)
{
	if (SpawnedActor)
	{
		SpawnedActor->GetProjectileMovementComponent()->InitialSpeed = ProjectileVelocity;
		SpawnedActor->GetProjectileMovementComponent()->PivotAxisStart = Location.GetTranslation();
		SpawnedActor->EffectSpecHandleFromAbility = GESpecHandle;

		SpawnedActor->FinishSpawning(Location);

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnSpawned.Broadcast(SpawnedActor);
		}
	}

	EndTask();
}
