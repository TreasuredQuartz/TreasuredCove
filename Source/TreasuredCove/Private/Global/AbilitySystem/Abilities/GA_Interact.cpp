// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Interact.h"
#include "GACharacter.h"
#include "GAActor.h"

#include "SenseComponent.h"
#include "InteractionInterface.h"

#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

void UGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Anything before we Commit goes here

	if (CommitAbility(Handle, ActorInfo, ActivationInfo)) // Commit...
	{
		// ...Everything else we do goes here! Make sure to call EndAbility()!!!
		float Range = 800.f;
		FHitResult Hit;
		FVector Start = GetOwningCharacter()->Sight->GetComponentLocation();
		FVector End = Start + Range * GetOwningCharacter()->Sight->GetComponentRotation().Vector();
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
						AGACharacter* Character = GetOwningCharacter();

						if (Character)
						{
							AActor* Item = Character->CurrentInteractItem;

							if (Item)
							{
								IInteractionInterface::Execute_InteractedWith(Item, Character);
								Character->CurrentInteractItem = nullptr;
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
