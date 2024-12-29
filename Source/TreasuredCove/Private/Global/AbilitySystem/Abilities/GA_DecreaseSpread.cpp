// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/Abilities/GA_DecreaseSpread.h"
#include "Global/Actors/GACharacter.h"
#include "AbilitySystemComponent.h"

void UGA_DecreaseSpread::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Anything before we Commit goes here

	if (CommitAbility(Handle, ActorInfo, ActivationInfo)) // Commit...
	{
		// ...Everything else we do goes here! Make sure to call EndAbility()!!!

		FGameplayAttribute SpreadAngle;
		FGameplayAttribute SpreadMin;
		FGameplayAttribute SpreadDecreaseSpeed;

		UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get();

		if (AbilitySystem)
		{
			if (AbilitySystem->HasAttributeSetForAttribute(SpreadAngle) && 
				AbilitySystem->HasAttributeSetForAttribute(SpreadMin) &&
				AbilitySystem->HasAttributeSetForAttribute(SpreadDecreaseSpeed))
			{
				float Angle = AbilitySystem->GetNumericAttributeBase(SpreadAngle);
				float Min = AbilitySystem->GetNumericAttributeBase(SpreadMin);
				float Speed = AbilitySystem->GetNumericAttributeBase(SpreadDecreaseSpeed);

				if (Angle >= Min)
				{
					// This is running off tick in our character
					float DeltaTime = GetOwningCharacter()->DeltaTime;
					float DecreaseAmount = DeltaTime * Speed;

					if (Angle - DecreaseAmount <= Min)
					{
						Angle = Min;
					}
					else
					{
						Angle = Angle - DecreaseAmount;
					}

					AbilitySystem->SetNumericAttributeBase(SpreadAngle, Angle);
				}

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
