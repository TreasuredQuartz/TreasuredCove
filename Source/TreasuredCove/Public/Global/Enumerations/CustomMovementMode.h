// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectMacros.h"

/** Custom Movement modes for characters
 * 
 */
UENUM(BlueprintType)
enum ECustomMovementMode
{
	MOVE_WallRunning		UMETA(DisplayName = "WallRunning"),
	MOVE_Climbing			UMETA(DisplayName = "Climbing"),
	// MOVE_MAX				UMETA(Hidden),						// error: redefinition; previous definition was 'enumerator'
};
