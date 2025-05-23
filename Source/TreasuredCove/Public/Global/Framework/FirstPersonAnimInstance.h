// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FirstPersonAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UFirstPersonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
private:
	float CrouchAlpha;
	float WalkAnimAlpha;
	float DipAlpha;
	FVector WalkVector;
	FVector SprintVector;
	FVector LocationLagVector;
	FRotator WalkRotator;
	FRotator SprintRotator;
	EMovementMode MovementMode;
};
