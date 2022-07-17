// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PlaySoundAtLocation.generated.h"

class USoundAttenuation;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UAbilityTask_PlaySoundAtLocation : public UAbilityTask
{
	GENERATED_BODY()

	//
	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Ability|Tasks")
	static UAbilityTask_PlaySoundAtLocation* PlaySoundAtLocation(UGameplayAbility* OwningAbility, USoundBase* InSound, FVector InLocation, float InVolumeMultiplier, float InPitchMultiplier, float InStartTime, USoundAttenuation* InAttenuationSettings, USoundConcurrency* InConcurrenceySettings);

	virtual void Activate() override;

protected:
	USoundBase* SoundToPlay;
	FVector LocationOfSound;
	float VolumeMultiplier;
	float PitchMultiplier;
	float StartTime;
	USoundAttenuation* AttenuationSettings;
	USoundConcurrency* ConcurrencySettings;
};
