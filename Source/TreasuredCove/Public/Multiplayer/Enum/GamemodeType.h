// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGamemodeType : uint8
{
	SinglePlayer,

	FreeForAll,
	TeamDeathMatch,
	Conquest,

	Payload,
	KingOfTheHill,

	Risk,
	Villianous,

	MAX
};
