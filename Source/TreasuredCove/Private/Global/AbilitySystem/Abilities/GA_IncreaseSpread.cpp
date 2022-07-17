// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_IncreaseSpread.h"
#include "GACharacter.h"
#include "AbilitySystemComponent.h"

void UGA_IncreaseSpread::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Anything before we Commit goes here

	if (CommitAbility(Handle, ActorInfo, ActivationInfo)) // Commit...
	{
		// ...Everything else we do goes here! Make sure to call EndAbility()!!!

		UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get();

		FGameplayAttribute SpreadAngle;
		FGameplayAttribute SpreadMax;
		FGameplayAttribute SpreadIncreaseSpeed;

		if (AbilitySystem)
		{
			if (AbilitySystem->HasAttributeSetForAttribute(SpreadAngle) &&
				AbilitySystem->HasAttributeSetForAttribute(SpreadMax) &&
				AbilitySystem->HasAttributeSetForAttribute(SpreadIncreaseSpeed))
			{
				float Angle = AbilitySystem->GetNumericAttributeBase(SpreadAngle);
				float Max = AbilitySystem->GetNumericAttributeBase(SpreadMax);
				float Speed = AbilitySystem->GetNumericAttributeBase(SpreadIncreaseSpeed);

				if (Angle <= Max)
				{
					float DeltaTime = GetOwningCharacter()->DeltaTime;
					float IncreaseAmount = DeltaTime * Speed;

					if (Angle + IncreaseAmount >= Max)
					{
						Angle = Max;
					}
					else
					{
						Angle = Angle + IncreaseAmount;
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
