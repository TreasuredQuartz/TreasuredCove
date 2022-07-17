// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	Primary,
	Secondary,
	Switch,
	Throw,
	Melee,
	Interact,
	Ultimate,
	Passive,
	Reactive,
	Cumalitive
};
