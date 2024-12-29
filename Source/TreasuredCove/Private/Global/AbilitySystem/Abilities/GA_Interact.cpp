// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/Abilities/GA_Interact.h"
#include "Global/Actors/GACharacter.h"
#include "Global/Actors/GAActor.h"

#include "SenseReceiverComponent.h"
#include "Global/Interfaces/InteractionInterface.h"

#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Anything before we Commit goes here

	if (CommitAbility(Handle, ActorInfo, ActivationInfo)) // Commit...
	{
		// ...Everything else we do goes here! Make sure to call EndAbility()!!!
		float Range = 200.f;
		FHitResult Hit;
		FVector Start = GetOwningCharacter()->AISenses->GetSensorLocation(FName("SensorSight"));
		FVector End = Start + Range * GetOwningCharacter()->AISenses->GetSensorRotation(FName("SensorSight")).Vector();
		FCollisionQueryParams CollisionParems;

		DrawDebugLine(GetWorld(), Start, End, FColor::Purple);
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParems))
		{
			if (Hit.bBlockingHit)
			{
				DrawDebugPoint(GetWorld(), Hit.Location, 5, FColor::Purple);
				GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Emerald, TEXT("Hit something"));

				if (Hit.GetActor() && Hit.GetActor()->GetClass() && Hit.GetActor() != GetOwningActorFromActorInfo())
				{
					if (Hit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
					{
						GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Emerald, TEXT("Interacting???"));
						IInteractionInterface::Execute_InteractedWith(Hit.GetActor(), GetOwningActorFromActorInfo());
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
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}
