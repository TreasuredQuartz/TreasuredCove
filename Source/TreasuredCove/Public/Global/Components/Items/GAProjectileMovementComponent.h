// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAProjectileMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGAProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()
	
protected:
	/** Compute the acceleration that will be applied */
	virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const;

public:
	/** Acceleration applied for a set time after spawn */
	float PivotAxisRotationRate;

	/** The Location to apply acceleration */
	FVector PivotAxisStart;

	/** The Location's direction to apply acceleration */
	FVector PivotAxisDirection;

	/** The Location to apply directional acceleration's offset */
	FVector PivotAxisOffset;

	bool bEnablePivotAxis;
};
