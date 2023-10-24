// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilitySet.generated.h"

class UGameplayAbility;

/** Data for granting a UGameplayAbility.
*
*	Specifically Intended for use with UAbilitySet.
*/
USTRUCT(BlueprintType)
struct FAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

/** Container for FGameplayAbilitySpecHandles.
*
*	Specifically intended for use with UAbilitySet.
*/
USTRUCT(BlueprintType)
struct FAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const struct FGameplayAbilitySpecHandle& Handle);

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

/** A Container intended to be instanced and filled with data
*	partaining to granting UGameplayAbilities.
* 
*/
UCLASS(Blueprintable)
class TREASUREDCOVE_API UAbilitySet : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<FAbilitySet_GameplayAbility> Abilities;

public:
	TArray<TSubclassOf<UGameplayAbility>> GetAbilities() const;
	void GiveToAbilitySystem(class UAbilitySystemComponent* InASC, FAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;
};
