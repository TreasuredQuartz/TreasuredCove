// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeons/StarFox/Minecraft/Abilities/GA_MCWorld_SpawnItem.h"
#include "Dungeons/StarFox/Minecraft/MCWorld_VoxelItem.h"
#include "Dungeons/StarFox/Minecraft/MCWorld_Voxel.h"
#include "Abilities/Tasks/AbilityTask_SpawnActor.h"

#include "Global/Actors/GAActor.h"

void UGA_MCWorld_SpawnItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Anything before Commit gos here

	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		AMCWorld_Voxel* OwningChunk = Cast<AMCWorld_Voxel>(ActorInfo->OwnerActor);

		if (OwningChunk)
		{
			TSubclassOf<AMCWorld_VoxelItem> VoxelClass;
			FGameplayAbilityTargetDataHandle TargetData;

			UAbilityTask_SpawnActor* SpawnActorTask = UAbilityTask_SpawnActor::SpawnActor(this, TargetData, VoxelClass);

			AActor* SpawnedActor;
			UMaterialInterface* VoxelMaterial = OwningChunk->SpawnVoxelMaterial;

			if (SpawnActorTask->BeginSpawningActor(this, TargetData, VoxelClass, SpawnedActor))
			{
				AMCWorld_VoxelItem* VoxelItem = Cast<AMCWorld_VoxelItem>(SpawnedActor);

				if (VoxelItem)
				{
					if (VoxelMaterial)
					{
						VoxelItem->Material = VoxelMaterial;
						// VoxelItem->BlockData = ;
						VoxelItem->bMini = true;

						SpawnActorTask->FinishSpawningActor(this, TargetData, VoxelItem);
						EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
					}
				}
				else
				{
					SpawnActorTask->FinishSpawningActor(this, TargetData, SpawnedActor);
					EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
				}
			}
			else
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			}
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}