// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayAbilityBase.h"
#include "GALibrary.h"
#include "GAActor.h"
#include "GACharacter.h"

FAbilityInfo UGameplayAbilityBase::GetAbilityInfo()
{
	EAbilityCostType AbilityCostType = EAbilityCostType::Ammo;

	UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	UGameplayEffect* CostEffect = GetCostGameplayEffect();

	float CooldownDuration = 0;
	float Cost = 0;

	if (CooldownEffect && CostEffect)
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);

		if (CostEffect->Modifiers.Num() > 0)
		{
			FGameplayModifierInfo EffectInfo = CostEffect->Modifiers[0];
			EffectInfo.ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
			FGameplayAttribute CostAttribute = EffectInfo.Attribute;
			FString AttributeName = CostAttribute.AttributeName;

			UGALibrary::SwitchOnAttributeName(AttributeName, AbilityCostType);
		}
	}

	return FAbilityInfo(GetClass(), AbilityName, AbilityDescription, AbilityTexture, CooldownDuration, Cost, AbilityCostType);
}

void UGameplayAbilityBase::UICooldown()
{

}

AGACharacter* UGameplayAbilityBase::GetOwningCharacter()
{
	return Cast<AGACharacter>(GetOwningActorFromActorInfo());
}

AGAActor* UGameplayAbilityBase::GetOwningItem()
{
	return Cast<AGAActor>(GetOwningActorFromActorInfo());
}
