// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/AbilitySet.h"
#include "AbilitySystemComponent.h"

void FAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

TArray<TSubclassOf<UGameplayAbility>> UAbilitySet::GetAbilities() const
{
	TArray<TSubclassOf<UGameplayAbility>> OutAbilities;

	OutAbilities.Reserve(Abilities.Num());
	for (const FAbilitySet_GameplayAbility& Ability : Abilities)
	{
		OutAbilities.Add(Ability.Ability);
	}

	return OutAbilities;
}

void UAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* InASC, FAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(InASC);

	if (!InASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (int32 AbilityIndex = 0; AbilityIndex < Abilities.Num(); ++AbilityIndex)
	{
		const FAbilitySet_GameplayAbility& AbilityToGrant = Abilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = InASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}
