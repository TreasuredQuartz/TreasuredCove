// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "ItemMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UItemMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	UItemMovementComponent();

private:
	static const float MIN_TICK_TIME;
	static const float BRAKE_TO_STOP_VELOCITY;

private:
	float BrakingFrictionFactor;
	float BrakingSubStepTime;
	float FallingLateralFriction;
	float MaxSimulationTimeStep;
	int32 MaxSimulationIterations;
	FVector PendingLaunchVelocity;
	FVector Acceleration;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	// Movement functions broken out based on owner's network Role.
	// TickComponent calls the correct version based on the Role.
	// These may be called during move playback and correction during network updates.
	//

	/** Perform movement on an autonomous client */
	virtual void PerformMovement(float DeltaTime);

	/** Special Tick for Simulated Proxies */
	virtual void SimulatedTick(float DeltaTime);

	/** Simulate movement on a non-owning client. Called by SimulatedTick(). */
	virtual void SimulateMovement(float DeltaTime);

	/** Apply the correct physics function per the movement mode. */
	virtual void StartNewPhysics(float DeltaTime, int32 Iterations);

	/**/
	virtual void PhysFalling(float DeltaTime, int32 Iterations);

	/**/
	void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDecceleration);

	/**/
	void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration);

	/**/
	FVector NewFallVelocity(FVector Velocity, FVector Gravity, float GravityTime);

	/**/
	void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations);

	/**/
	float GetSimulationTimeStep(float DelatTime, int32 Iterations);

public:
	bool HasValidData() const;
	bool IsValidLandingSpot(const FVector& Location, FHitResult& Hit) const;
	FVector GetFallingLateralAcceleration(float DeltaTime) const;
	float GetMaxBrakingDeceleration() const;

public:
	void Launch(FVector LaunchVelocity);
	bool HandlePendingLaunch();

};
