// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySet.generated.h"

class UGameplayAbility;

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UAbilitySet : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
public:
	TArray<TSubclassOf<UGameplayAbility>> GetAbilities() const;
};
