// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ERelationship : uint8
{
	Stranger,
	SoulBonded,
	Lover,
	Partner,
	Friend,
	Aquaintance,
	Neutral,
	Rival,
	Adversary,
	Nemesis,
	Hated
};
