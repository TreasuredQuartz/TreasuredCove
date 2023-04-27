// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "GA_MCWorld_PlaceVoxel.generated.h"

class AMCWorld_Voxel;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGA_MCWorld_PlaceVoxel : public UGameplayAbilityBase
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
