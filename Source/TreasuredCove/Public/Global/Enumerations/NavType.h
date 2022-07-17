// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ENavType : uint8
{
	Up,
	Right,
	Down,
	Left, 
	Front,
	Back
};
