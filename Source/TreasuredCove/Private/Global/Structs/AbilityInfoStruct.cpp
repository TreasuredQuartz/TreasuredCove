// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityInfoStruct.h"
#include "GameplayAbilityBase.h"

FAbilityInfo::FAbilityInfo()
	: Class(nullptr)
	, Name("")
	, Description(FText())
	, UIMaterial(nullptr)
	, Cooldown(0)
	, Cost(0)
	, CostType(EAbilityCostType::Health)
	, RequiredAbilities({nullptr})
{
}

FAbilityInfo::FAbilityInfo(float InCoolDown, float InCost, EAbilityCostType InAbilityCostType,
	UMaterialInterface* InMaterial, TSubclassOf<UGameplayAbilityBase> InAbilityClass, TArray<TSubclassOf<UGameplayAbilityBase>> InRequiredAbilities,
	TArray<FRequiredAttribute> InRequiredAttributes, FName InAbilityName, FText InAbilityDescription)
	: Class(InAbilityClass)
	, Name(InAbilityName)
	, Description(InAbilityDescription)
	, UIMaterial(InMaterial)
	, Cooldown(InCoolDown)
	, Cost(InCost)
	, CostType(InAbilityCostType)
	, RequiredAbilities(InRequiredAbilities)
	, RequiredAttributes(InRequiredAttributes)
{
}