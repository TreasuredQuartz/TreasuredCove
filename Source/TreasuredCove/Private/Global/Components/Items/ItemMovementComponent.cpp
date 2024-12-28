// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Components/Items/ItemMovementComponent.h"
#include "Global/Actors/GAActor.h"

// Engine Includes
#include "GameFramework/PhysicsVolume.h"

const float UItemMovementComponent::MIN_TICK_TIME = 1e-6f;
const float UItemMovementComponent::BRAKE_TO_STOP_VELOCITY = 10.f;

UItemMovementComponent::UItemMovementComponent()
{
	MaxSimulationTimeStep = 0.05;
	MaxSimulationIterations = 8;
	BrakingFrictionFactor = 1.0f;
	BrakingSubStepTime = 1.0f / 33.0f;
	PendingLaunchVelocity = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
}

void UItemMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Super tick may destroy/invalidate CharacterOwner or UpdatedComponent, so we need to re-check.
	if (!HasValidData())
	{
		return;
	}

	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		SimulatedTick(DeltaTime);
	}

	SimulatedTick(DeltaTime);
}

void UItemMovementComponent::PerformMovement(float DeltaTime)
{
	const UWorld* MyWorld = GetWorld();
	if (!HasValidData() || MyWorld == nullptr)
	{
		return;
	}

	FVector OldVelocity;
	FVector OldLocation;
	
	{
		// Launch Item
		HandlePendingLaunch();

		// Physics
		StartNewPhysics(DeltaTime, 0);

		// Call external post-movement events. These happen after the scoped movement completes in case the events want to use the current state of overlaps etc.
		// CallMovementUpdateDelegate(DeltaTime, OldLocation, OldVelocity);
	}

	UpdateComponentVelocity();
}

void UItemMovementComponent::SimulatedTick(float DeltaTime)
{
	if (GetOwner()->IsReplicatingMovement() && UpdatedComponent)
	{
		// Avoid moving the mesh during movement if SmoothClientPosition will take care of it.
		{
			{
				PerformMovement(DeltaTime);
			}
			if (false)
			{
				SimulateMovement(DeltaTime);
			}
		}
	}
}

void UItemMovementComponent::SimulateMovement(float DeltaTime)
{
	if (!HasValidData() || UpdatedComponent->Mobility != EComponentMobility::Movable || UpdatedComponent->IsSimulatingPhysics())
	{
		return;
	}

	HandlePendingLaunch();
}

void UItemMovementComponent::StartNewPhysics(float DeltaTime, int32 Iterations)
{
	PhysFalling(DeltaTime, Iterations);
}

void UItemMovementComponent::PhysFalling(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	bool bJustTeleported = false;
	FVector FallAcceleration = GetFallingLateralAcceleration(DeltaTime);
	FallAcceleration.Z = 0.f;

	float remainingTime = DeltaTime;
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations))
	{
		Iterations++;
		float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FQuat PawnRotation = UpdatedComponent->GetComponentQuat();
		const FVector OldVelocity = Velocity;

		const float MaxDecel = GetMaxBrakingDeceleration();
		// Compute Velocity
		{
			// Acceleration = FallAcceleration for CalcVelocity(), but we restore it after using it.
			TGuardValue<FVector> RestoreAcceleration(Acceleration, FallAcceleration);
			Velocity.Z = 0.f;
			CalcVelocity(timeTick, FallingLateralFriction, false, MaxDecel);
			Velocity.Z = OldVelocity.Z;
		}

		// Compute current gravity
		const FVector Gravity(0.f, 0.f, GetGravityZ());
		float GravityTime = timeTick;

		// Apply gravity
		Velocity = NewFallVelocity(Velocity, Gravity, GravityTime);

		// DecayFormerBaseVelocity(timeTick);

		// Compute change in position (using midpoint integration method).
		FVector Adjusted = 0.5f * (Velocity) * timeTick;

		// Move
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Adjusted, PawnRotation, true, Hit);

		if (!HasValidData())
		{
			return;
		}

		float LastMoveTimeSlice = timeTick;
		float subTimeTickRemaining = timeTick * (1.f - Hit.Time);

		if (Hit.bBlockingHit && false)
		{
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
			{
				remainingTime += subTimeTickRemaining;
				ProcessLanded(Hit, remainingTime, Iterations);
				return;
			}
			else
			{
				// Compute impact deflection based on final velocity, not integration step.
				// This allows us to compute a new velocity from the deflected vector, and ensures the full gravity effect is included in the slide result.
				Adjusted = Velocity * timeTick;

				// See if we can convert a normally invalid landing spot (based on the hit result) to a usable one.
				/*if (!Hit.bStartPenetrating && ShouldCheckForValidLandingSpot(timeTick, Adjusted, Hit))
				{
					const FVector PawnLocation = UpdatedComponent->GetComponentLocation();
					FFindFloorResult FloorResult;
					FindFloor(PawnLocation, FloorResult, false);
					if (FloorResult.IsWalkableFloor() && IsValidLandingSpot(PawnLocation, FloorResult.HitResult))
					{
						remainingTime += subTimeTickRemaining;
						ProcessLanded(FloorResult.HitResult, remainingTime, Iterations);
						return;
					}
				}*/

				HandleImpact(Hit, LastMoveTimeSlice, Adjusted);

				// If we've changed physics mode, abort.
				if (!HasValidData())
				{
					return;
				}

				const FVector OldHitNormal = Hit.Normal;
				const FVector OldHitImpactNormal = Hit.ImpactNormal;
				FVector Delta = ComputeSlideVector(Adjusted, 1.f - Hit.Time, OldHitNormal, Hit);

				// Compute velocity after deflection (only gravity component for RootMotion)
				const UPrimitiveComponent* HitComponent = Hit.GetComponent();
				if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && !bJustTeleported)
				{
					const FVector NewVelocity = (Delta / subTimeTickRemaining);
					Velocity = NewVelocity;
				}

				if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && (Delta | Adjusted) > 0.f)
				{
					// Move in deflected direction.
					SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);

					if (Hit.bBlockingHit)
					{
						// hit second wall
						LastMoveTimeSlice = subTimeTickRemaining;
						subTimeTickRemaining = subTimeTickRemaining * (1.f - Hit.Time);

						if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
						{
							remainingTime += subTimeTickRemaining;
							ProcessLanded(Hit, remainingTime, Iterations);
							return;
						}

						HandleImpact(Hit, LastMoveTimeSlice, Delta);

						// If we've changed physics mode, abort.
						if (!HasValidData())
						{
							return;
						}

						FVector PreTwoWallDelta = Delta;
						TwoWallAdjust(Delta, Hit, OldHitNormal);


						if (subTimeTickRemaining > UE_KINDA_SMALL_NUMBER && !bJustTeleported)
						{
							const FVector NewVelocity = (Delta / subTimeTickRemaining);
							Velocity = NewVelocity;
						}

						// bDitch=true means that pawn is straddling two slopes, neither of which it can stand on
						bool bDitch = ((OldHitImpactNormal.Z > 0.f) && (Hit.ImpactNormal.Z > 0.f) && (FMath::Abs(Delta.Z) <= UE_KINDA_SMALL_NUMBER) && ((Hit.ImpactNormal | OldHitImpactNormal) < 0.f));
						SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);
						if (Hit.Time == 0.f)
						{
							// if we are stuck then try to side step
							FVector SideDelta = (OldHitNormal + Hit.ImpactNormal).GetSafeNormal2D();
							if (SideDelta.IsNearlyZero())
							{
								SideDelta = FVector(OldHitNormal.Y, -OldHitNormal.X, 0).GetSafeNormal();
							}
							SafeMoveUpdatedComponent(SideDelta, PawnRotation, true, Hit);
						}

						if (bDitch || IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit) || Hit.Time == 0.f)
						{
							remainingTime = 0.f;
							ProcessLanded(Hit, remainingTime, Iterations);
							return;
						}
					}
				}
			}
		}

		if (Velocity.SizeSquared2D() <= UE_KINDA_SMALL_NUMBER * 10.f)
		{
			Velocity.X = 0.f;
			Velocity.Y = 0.f;
		}
	}
}

void UItemMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	// Do not update velocity when using root motion or when SimulatedProxy and not simulating root motion - SimulatedProxy are repped their Velocity
	if (!HasValidData() || DeltaTime < MIN_TICK_TIME || (GetOwner() && GetOwner()->GetLocalRole() == ROLE_SimulatedProxy))
	{
		return;
	}

	Friction = FMath::Max(0.f, Friction);
	float MaxSpeed = GetMaxSpeed();

	// Path following above didn't care about the analog modifier, but we do for everything else below, so get the fully modified value.
	// Use max of requested speed and max speed if we modified the speed in ApplyRequestedMove above.

	// Apply braking or deceleration
	const bool bVelocityOverMax = IsExceedingMaxSpeed(MaxSpeed);

	// Only apply braking if there is no acceleration, or we are over our max speed and need to slow down to it.
	if (false)
	{
		const FVector OldVelocity = Velocity;

		const float ActualBrakingFriction = Friction;
		ApplyVelocityBraking(DeltaTime, ActualBrakingFriction, BrakingDeceleration);

		// Don't allow braking to lower us below max speed if we started above it.
		if (bVelocityOverMax && Velocity.SizeSquared() < FMath::Square(MaxSpeed) && FVector::DotProduct(Acceleration, OldVelocity) > 0.0f)
		{
			Velocity = OldVelocity.GetSafeNormal() * MaxSpeed;
		}
	}
}

void UItemMovementComponent::ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration)
{
	if (Velocity.IsZero() || !HasValidData() || DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	const float FrictionFactor = FMath::Max(0.f, BrakingFrictionFactor);
	Friction = FMath::Max(0.f, Friction * FrictionFactor);
	BrakingDeceleration = FMath::Max(0.f, BrakingDeceleration);
	const bool bZeroFriction = (Friction == 0.f);
	const bool bZeroBraking = (BrakingDeceleration == 0.f);

	if (bZeroFriction && bZeroBraking)
	{
		return;
	}

	const FVector OldVel = Velocity;

	// subdivide braking to get reasonably consistent results at lower frame rates
	// (important for packet loss situations w/ networking)
	float RemainingTime = DeltaTime;
	const float MaxTimeStep = FMath::Clamp(BrakingSubStepTime, 1.0f / 75.0f, 1.0f / 20.0f);

	// Decelerate to brake to a stop
	const FVector RevAccel = (bZeroBraking ? FVector::ZeroVector : (-BrakingDeceleration * Velocity.GetSafeNormal()));
	while (RemainingTime >= MIN_TICK_TIME)
	{
		// Zero friction uses constant deceleration, so no need for iteration.
		const float dt = ((RemainingTime > MaxTimeStep && !bZeroFriction) ? FMath::Min(MaxTimeStep, RemainingTime * 0.5f) : RemainingTime);
		RemainingTime -= dt;

		// apply friction and braking
		Velocity = Velocity + ((-Friction) * Velocity + RevAccel) * dt;

		// Don't reverse direction
		if ((Velocity | OldVel) <= 0.f)
		{
			Velocity = FVector::ZeroVector;
			return;
		}
	}

	// Clamp to zero if nearly zero, or if below min threshold and braking.
	const float VSizeSq = Velocity.SizeSquared();
	if (VSizeSq <= UE_KINDA_SMALL_NUMBER || (!bZeroBraking && VSizeSq <= FMath::Square(BRAKE_TO_STOP_VELOCITY)))
	{
		Velocity = FVector::ZeroVector;
	}
}

FVector UItemMovementComponent::NewFallVelocity(FVector InitialVelocity, FVector Gravity, float DeltaTime)
{
	FVector Result = InitialVelocity;

	if (DeltaTime > 0.f)
	{
		// Apply gravity.
		Result += Gravity * DeltaTime;

		// Don't exceed terminal velocity.
		const float TerminalLimit = FMath::Abs(GetPhysicsVolume()->TerminalVelocity);
		if (Result.SizeSquared() > FMath::Square(TerminalLimit))
		{
			const FVector GravityDir = Gravity.GetSafeNormal();
			if ((Result | GravityDir) > TerminalLimit)
			{
				Result = FVector::PointPlaneProject(Result, FVector::ZeroVector, GravityDir) + GravityDir * TerminalLimit;
			}
		}
	}

	return Result;
}

FVector UItemMovementComponent::GetFallingLateralAcceleration(float DeltaTime) const
{
	// No acceleration in Z
	FVector FallAcceleration = FVector(Acceleration.X, Acceleration.Y, 0.f);

	// bound acceleration, falling object has minimal ability to impact acceleration
	if (FallAcceleration.SizeSquared2D() > 0.f)
	{
		// FallAcceleration = GetAirControl(DeltaTime, AirControl, FallAcceleration);
		// FallAcceleration = FallAcceleration.GetClampedToMaxSize(GetMaxAcceleration());
	}

	return FallAcceleration;
}

float UItemMovementComponent::GetMaxBrakingDeceleration() const
{
	return 1.0f;
}

void UItemMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	/*if (IsFalling())
	{
		SetPostLandedPhysics(Hit);


		const FVector PreImpactAccel = Acceleration + (IsFalling() ? FVector(0.f, 0.f, GetGravityZ()) : FVector::ZeroVector);
		const FVector PreImpactVelocity = Velocity;
		ApplyImpactPhysicsForces(Hit, PreImpactAccel, PreImpactVelocity);
		if (bEnablePhysicsInteraction && Impact.bBlockingHit)
		{
			if (UPrimitiveComponent* ImpactComponent = Impact.GetComponent())
			{
				FVector ForcePoint = Impact.ImpactPoint;
				float BodyMass = 1.0f; // set to 1 as this is used as a multiplier

				bool bCanBePushed = false;
				FBodyInstance* BI = ImpactComponent->GetBodyInstance(Impact.BoneName);
				if (BI != nullptr && BI->IsInstanceSimulatingPhysics())
				{
					BodyMass = FMath::Max(BI->GetBodyMass(), 1.0f);

					if (bPushForceUsingZOffset)
					{
						FBox Bounds = BI->GetBodyBounds();

						FVector Center, Extents;
						Bounds.GetCenterAndExtents(Center, Extents);

						if (!Extents.IsNearlyZero())
						{
							ForcePoint.Z = Center.Z + Extents.Z * PushForcePointZOffsetFactor;
						}
					}

					bCanBePushed = true;
				}
				else if (CharacterMovementCVars::bGeometryCollectionImpulseWorkAround)
				{
					const FName ClassName = ImpactComponent->GetClass()->GetFName();
					const FName GeometryCollectionClassName("GeometryCollectionComponent");
					if (ClassName == GeometryCollectionClassName && ImpactComponent->BodyInstance.bSimulatePhysics)
					{
						// in some case GetBodyInstance can return null while the BodyInstance still exists ( geometry collection component for example )
						// but we cannot check for its component directly here because of modules cyclic dependencies
						// todo(chaos): change this logic to be more driven at the primitive component level to avoid the high level classes to have to be aware of the different component

						// because of the above limititation we have to ignore bPushForceUsingZOffset

						bCanBePushed = true;
					}
				}

				if (bCanBePushed)
				{
					FVector Force = Impact.ImpactNormal * -1.0f;

					float PushForceModificator = 1.0f;

					const FVector ComponentVelocity = ImpactComponent->GetPhysicsLinearVelocity();
					const FVector VirtualVelocity = ImpactAcceleration.IsZero() ? ImpactVelocity : ImpactAcceleration.GetSafeNormal() * GetMaxSpeed();

					float Dot = 0.0f;

					if (bScalePushForceToVelocity && !ComponentVelocity.IsNearlyZero())
					{
						Dot = ComponentVelocity | VirtualVelocity;

						if (Dot > 0.0f && Dot < 1.0f)
						{
							PushForceModificator *= Dot;
						}
					}

					if (bPushForceScaledToMass)
					{
						PushForceModificator *= BodyMass;
					}

					Force *= PushForceModificator;
					const float ZeroVelocityTolerance = 1.0f;
					if (ComponentVelocity.IsNearlyZero(ZeroVelocityTolerance))
					{
						Force *= InitialPushForceFactor;
						ImpactComponent->AddImpulseAtLocation(Force, ForcePoint, Impact.BoneName);
					}
					else
					{
						Force *= PushForceFactor;
						ImpactComponent->AddForceAtLocation(Force, ForcePoint, Impact.BoneName);
					}
				}
			}
		}
	} // */

	StartNewPhysics(remainingTime, Iterations);
}

float UItemMovementComponent::GetSimulationTimeStep(float RemainingTime, int32 Iterations)
{
	static uint32 s_WarningCount = 0;
	if (RemainingTime > MaxSimulationTimeStep)
	{
		if (Iterations < MaxSimulationIterations)
		{
			// Subdivide moves to be no longer than MaxSimulationTimeStep seconds
			RemainingTime = FMath::Min(MaxSimulationTimeStep, RemainingTime * 0.5f);
		}
		else
		{
			// If this is the last iteration, just use all the remaining time. This is usually better than cutting things short, as the simulation won't move far enough otherwise.
			// Print a throttled warning.
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			if ((s_WarningCount++ < 100) || (GFrameCounter & 15) == 0)
			{
				// UE_LOG(LogCharacterMovement, Warning, TEXT("GetSimulationTimeStep() - Max iterations %d hit while remaining time %.6f > MaxSimulationTimeStep (%.3f) for '%s', movement '%s'"), MaxSimulationIterations, RemainingTime, MaxSimulationTimeStep, *GetNameSafe(CharacterOwner), *GetMovementName());
			}
#endif
		}
	}

	// no less than MIN_TICK_TIME (to avoid potential divide-by-zero during simulation).
	return FMath::Max(MIN_TICK_TIME, RemainingTime);
}

void UItemMovementComponent::Launch(FVector LaunchVelocity)
{
	if (IsActive() && HasValidData())
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemMovementComponent: LaunchItem"));
		PendingLaunchVelocity = LaunchVelocity;
	}
}

bool UItemMovementComponent::HandlePendingLaunch()
{
	if (!PendingLaunchVelocity.IsZero() && HasValidData())
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemMovementComponent: HandlePendingLaunch"));
		Velocity = PendingLaunchVelocity;
		PendingLaunchVelocity = FVector::ZeroVector;
		return true;
	}

	return false;
}

bool UItemMovementComponent::HasValidData() const
{
	const bool bIsValid = UpdatedComponent && IsValid(GetOwner());
	return bIsValid;
}

bool UItemMovementComponent::IsValidLandingSpot(const FVector& Location, FHitResult& Hit) const
{
	if (!Hit.bBlockingHit)
	{
		return false;
	}

	// Skip some checks if penetrating. Penetration will be handled by the FindFloor call (using a smaller capsule)
	if (!Hit.bStartPenetrating)
	{
		auto IsWalkable = [](FHitResult& Hit)
		{
			if (!Hit.IsValidBlockingHit())
			{
				// No hit, or starting in penetration
				return false;
			}

			// Never walk up vertical surfaces.
			if (Hit.ImpactNormal.Z < UE_KINDA_SMALL_NUMBER)
			{
				return false;
			}

			float TestWalkableZ = 0.71f;

			// See if this component overrides the walkable floor z.
			const UPrimitiveComponent* HitComponent = Hit.Component.Get();
			if (HitComponent)
			{
				const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
				TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
			}

			// Can't walk on this surface if it is too steep.
			if (Hit.ImpactNormal.Z < TestWalkableZ)
			{
				return false;
			}

			return true;
		};

		// Reject unwalkable floor normals.
		if (!IsWalkable(Hit))
		{
			return false;
		}
	}
	else
	{
		// Penetrating
		if (Hit.Normal.Z < UE_KINDA_SMALL_NUMBER)
		{
			// Normal is nearly horizontal or downward, that's a penetration adjustment next to a vertical or overhanging wall. Don't pop to the floor.
			return false;
		}
	}

	return true;
}
