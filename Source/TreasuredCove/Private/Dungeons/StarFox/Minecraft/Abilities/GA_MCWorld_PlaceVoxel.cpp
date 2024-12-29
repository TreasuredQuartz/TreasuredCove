// Fill out your copyright notice in the Description page of Project Settings.


#include "Dungeons/StarFox/Minecraft/Abilities/GA_MCWorld_PlaceVoxel.h"
#include "Dungeons/StarFox/Minecraft/MCWorld_BasicInteractionTool.h"
#include "Dungeons/StarFox/Minecraft/MCWorld_Voxel.h"

#include "Global/Actors/GACharacter.h"

#include "SenseReceiverComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

void UGA_MCWorld_PlaceVoxel::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Anything before Commit gos here

	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		float Range = 500.f;
		FHitResult Hit;
		FVector Start = GetOwningCharacter()->AISenses->GetComponentLocation();
		FVector End = Start + (Range * GetOwningCharacter()->AISenses->GetComponentRotation().Vector());
		FCollisionQueryParams CollisionParems;

		DrawDebugLine(GetWorld(), Start, End, FColor::Purple);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParems))
		{
			if (Hit.bBlockingHit)
			{
				DrawDebugPoint(GetWorld(), Hit.Location, 5, FColor::Purple);

				AMCWorld_Voxel* HitVoxel = Cast<AMCWorld_Voxel>(Hit.GetActor());
				if (HitVoxel)
				{
					FVector Direction =
						(Hit.Location - Start).GetSafeNormal();

					FVector Vector2 = Hit.Location + (Direction * 1);

					HitVoxel->SetVoxel(Vector2, 1);

					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Placed!"));

					EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
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
	else 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

}