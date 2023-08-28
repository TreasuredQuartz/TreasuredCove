// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityInfoStruct.h"
#include "GameplayAbilityBase.h"

FAbilityInfo::FAbilityInfo()
	: Name("")
	, Class(nullptr)
	, Description(FText())
	, Texture(nullptr)
	, Cooldown(0)
	, Cost(0)
	, CostType(EAbilityCostType::Health)
{
}

FAbilityInfo::FAbilityInfo(TSubclassOf<UGameplayAbilityBase> InAbilityClass, FName InAbilityName, FText InAbilityDescription, UTexture2D* InTexture,
	float InCoolDown, float InCost, EAbilityCostType InAbilityCostType)
	: Name(InAbilityName)
	, Class(InAbilityClass)
	, Description(InAbilityDescription)
	, Texture(InTexture)
	, Cooldown(InCoolDown)
	, Cost(InCost)
	, CostType(InAbilityCostType)
{
}