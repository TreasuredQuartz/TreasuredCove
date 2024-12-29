// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/AbilitySystem/AbilityTasks/AbilityTask_PlaySoundAtLocation.h"
#include "Kismet/GameplayStatics.h"

UAbilityTask_PlaySoundAtLocation* UAbilityTask_PlaySoundAtLocation::PlaySoundAtLocation(UGameplayAbility* OwningAbility, USoundBase* InSound, FVector InLocation, float InVolumeMultiplier = 1.f, float InPitchMultiplier = 1.f, float InStartTime = 1.f, USoundAttenuation* InAttenuationSettings = nullptr, USoundConcurrency* InConcurrencySettings = nullptr)
{
	UAbilityTask_PlaySoundAtLocation* MyObj = NewAbilityTask<UAbilityTask_PlaySoundAtLocation>(OwningAbility);
	MyObj->SoundToPlay = InSound;
	MyObj->LocationOfSound = InLocation;
	MyObj->VolumeMultiplier = InVolumeMultiplier;
	MyObj->PitchMultiplier = InPitchMultiplier;
	MyObj->StartTime = InStartTime;
	MyObj->AttenuationSettings = InAttenuationSettings;
	MyObj->ConcurrencySettings = InConcurrencySettings;
	return MyObj;
}

void UAbilityTask_PlaySoundAtLocation::Activate()
{
	if (Ability && Ability->GetCurrentActorInfo()->IsNetAuthority() && ShouldBroadcastAbilityTaskDelegates())
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			UGameplayStatics::PlaySoundAtLocation(World, SoundToPlay, LocationOfSound, FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier, StartTime, AttenuationSettings, ConcurrencySettings, GetOwnerActor());
		}
	}
}
