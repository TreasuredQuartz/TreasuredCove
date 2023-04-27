// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleMovementComponent.h"
#include "FlyingVehicleMovementComponent.generated.h"

USTRUCT()
struct PHYSXFLYINGVEHICLES_API FAirFoil
{
	GENERATED_BODY()

	/* L = Cl * A * 0.5 * r * V ^ 2
	* Where L is Lift, Cl is the Lift Coefficient,
	* A is the area of the wing, r is the density of the air,
	* and V is the velocity of the wing.
	*/
	float Lift;						// L
	float LiftCoefficient;			// Cl
	float AirDensity;				// r
	float AirfoilArea;				// A
	
	FVector ChordLine;				// The line drawn from the left most to the right most part of the wing
	float Chamber;					// The measure of the curvature of the airfoil
	float Thickness;				// The maximum height between the upper and lower surface of the airfoil
	FVector AirDirection;			// The angle that the air is moving toward the wing

	float AngleOfAttack;			// The angle between the chord line and the flight direction

	float CylinderRadius;			// 
	FVector2D CylanderLocation;		//
	float CylinderCirculation;		//
	float WingSpan;
	float Chord;
};

/**
 * 
 */
UCLASS()
class PHYSXFLYINGVEHICLES_API UFlyingVehicleMovementComponent : 
	public UVehicleMovementComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Transient)
	uint8 bThrottleUp : 1;
	UPROPERTY(Transient)
	uint8 bThrottleDown : 1;

	int32 ThrottleAmount;
public:
	// Construct
	UFlyingVehicleMovementComponent();

	// Primary Update function
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector WingForceVector;
	FVector WingNormalDirectionVector;
	float Speed;
	FVector ForwardVector;

	//////////////////////////////////////////////////////
	/////				INPUT VARIABLES				//////
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	/////				OUTPUT VARIABLES			//////
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	/////			FLUID DYNAMICS EQUATIONS		//////
	//////////////////////////////////////////////////////

	float CalcWeight(float InMass, float InGravity = 9.8f)
	{
		return InMass * InGravity;
	}

	float CalcDynamicPressure(float InDensity, float InVelocity)
	{
		return 0.5f * InDensity * pow(InVelocity, 2);
	}

	float CalcFlowSpeed(float Position, float Time)
	{
		FVector2D FlowVelocity = FVector2D(Position, Time);
		return FlowVelocity.Size();
	}

	float CalcDrag(float InCoefficient, float InDynamicPressure, float InArea)
	{
		return InCoefficient * InDynamicPressure * InArea;
	}

	float CalcDragCoefficient(float InDrag, float InDynamicPressure, float InArea)
	{
		return InDrag / (InDynamicPressure * InArea);
	}

	float CalcDragCoefficient(float InZeroLiftDragCoefficient, float InInducedDragCoefficient)
	{
		return InZeroLiftDragCoefficient + InInducedDragCoefficient;
	}

	float CalcInducedDragCoefficient(float InLiftCoefficient, float InAspectRatio, float InEfficiencyFactor = 0.70f)
	{
		return pow(InLiftCoefficient, 2) / (3.14159 * InAspectRatio * InEfficiencyFactor);
	}

	float CalcAspectRatio(float InSpan, float InArea)
	{
		return pow(InSpan, 2) / InArea;
	}

	float CalcLift(float InCoefficient, float InDynamicPressure)
	{
		return InCoefficient * InDynamicPressure;
	}

	float CalcLiftCoefficient(float InAngleOfAttack)
	{
		return 2 * 3.14159 * InAngleOfAttack;
	}

	float CalcReynoldsNumber(float InVelocity, float InDensity, float InCharacteristicLength, float InViscosityCoefficient)
	{
		return InVelocity * InDensity * InCharacteristicLength / InViscosityCoefficient;
	}
};
