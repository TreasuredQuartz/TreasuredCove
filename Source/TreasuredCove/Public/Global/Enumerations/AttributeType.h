// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
	Health,
	MaxHealth,
	Stamina,
	MaxStamina,
	Mana,
	MaxMana,
	Experience,
	MaxExperience
};
