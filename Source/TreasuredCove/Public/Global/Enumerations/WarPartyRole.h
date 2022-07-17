// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWarPartyRole : uint8
{
	Leader,
	Frontline,
	Guard,
	Bully,
	Recon,
	Healer,
	Utility
};
