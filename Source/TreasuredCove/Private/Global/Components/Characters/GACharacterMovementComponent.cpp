// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/GACharacterMovementComponent.h"
#include "GACharacter.h"
#include "CustomMovementMode.h"
#include "Components/CapsuleComponent.h"

// Framework
#include "GameFramework/Character.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"

// Engine
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

#pragma region Defaults
FNetworkPredictionData_Client* UGACharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		// Return our custom client prediction class instead
		UGACharacterMovementComponent* MutableThis = const_cast<UGACharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_GA(*this);
	}

	return ClientPredictionData;
}

void UGACharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultGroundFriction = GroundFriction;
	bDefaultOrientRotationToMovement = bOrientRotationToMovement;
	bDefaultUseControllerRotationYaw = CharacterOwner->bUseControllerRotationYaw;
	DefaultMeshRelativeLocation = CharacterOwner->GetMesh()->GetRelativeLocation();

	// If the client's role is higher than a simulated proxy
	// - A simulated proxy is another character not controlled by this client.
	// - Roles higher than this are AutonomousProxy, and Authoritive
	if (GetPawnOwner() && GetPawnOwner()->GetLocalRole() > ROLE_SimulatedProxy)
	{
		// GetPawnOwner()->InputComponent->Bind
		GetPawnOwner()->OnActorHit.AddDynamic(this, &UGACharacterMovementComponent::OnActorHit);
	}
}

void UGACharacterMovementComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (GetPawnOwner() != nullptr && GetPawnOwner()->GetLocalRole() > ROLE_SimulatedProxy)
	{
		// Unbind from all events
		GetPawnOwner()->OnActorHit.RemoveDynamic(this, &UGACharacterMovementComponent::OnActorHit);
	}

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UGACharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	
	if (GetPawnOwner()->IsLocallyControlled())
	{
		if (CanEverSprint())
		{
			if (bSprintKeyDown)
			{
				FVector velocity2D = GetPawnOwner()->GetVelocity();
				FVector forward2D = GetPawnOwner()->GetActorForwardVector();
				velocity2D.Z = 0.0f;
				forward2D.Z = 0.0f;
				velocity2D.Normalize();
				forward2D.Normalize();

				bWantsToSprint = FVector::DotProduct(velocity2D, forward2D) > 0.5f;
			}
			else
			{
				bWantsToSprint = false;
			}
		}

		if (CanEverSlide())
		{
			if (IsSliding())
			{
				if (Velocity.Size() <= MaxWalkSpeedCrouched) StopSliding();

				const bool bIsCrouching = IsCrouching();
				if (!bIsCrouching)
				{
					StopSliding();
				}

				const FVector Force = CalculateFloorInfluence(CurrentFloor.HitResult.Normal);
				AddForce(Force);
			}
			else if (bWantsToSlide && IsMovingOnGround())
			{
				StartSliding();
			}

			if (bConsecutiveSlidesLoseMomentum)
			{
				if (CurrentSlides > 0)
				{
					SlideConsecutiveResetDuration += DeltaTime;

					if (SlideConsecutiveResetDuration >= SlideConsecutiveResetDelay)
					{
						ResetSliding();
					}
				}
			}
		}

		// Update if required keys are down
		bForwardKeysAreDown = AreRequiredKeysDown();
	}

	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
	{
		GACharacter->bIsSprinting = bWantsToSprint;

		if (GACharacter->bFirstPerson)
		{
			FVector CurLocation = GACharacter->GetMesh()->GetRelativeLocation();
			FVector NewLocation = CurLocation;
			if (IsCrouching())
			{
				NewLocation.Z = FMath::FInterpTo(CurLocation.Z, -GetCrouchedHalfHeight() * 2, DeltaTime, 10.f);
			}
			else
			{
				NewLocation.Z = FMath::FInterpTo(CurLocation.Z, DefaultMeshRelativeLocation.Z, DeltaTime, 10.f);
			}

			GACharacter->GetMesh()->SetRelativeLocation(NewLocation);
		}
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGACharacterMovementComponent::UpdateFromCompressedFlags(uint8 flags)
{
	Super::UpdateFromCompressedFlags(flags);

	/* There are 4 custom move flags for us to use. Below is what each is currently being used for:
	FLAG_Custom_0		= 0x10, // Sprinting
	FLAG_Custom_1		= 0x20, // WallRunning
	FLAG_Custom_2		= 0x40, // Unused
	FLAG_Custom_3		= 0x80, // Unused
	*/

	//
	bWantsToSprint = (flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bForwardKeysAreDown = (flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

void UGACharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	bPrevOrientRotationToMovement = bOrientRotationToMovement;
	bPrevUseControllerRotationYaw = CharacterOwner->bUseControllerRotationYaw;
	
	switch (MovementMode)
	{
	case MOVE_Custom:
		switch (CustomMovementMode)
		{
		case ECustomMovementMode::MOVE_WallRunning:
			// Stop current Movement
			// StopMovementImmediately();
			CharacterOwner->bUseControllerRotationYaw = true;
			bOrientRotationToMovement = true;
			break;
		case ECustomMovementMode::MOVE_Climbing:
			// UE_LOG(LogTemp, Warning, TEXT("Custom Movement Mode changed to Climbing."));
			// Stop current Movement
			StopMovementImmediately();
			CharacterOwner->bUseControllerRotationYaw = false;
			bOrientRotationToMovement = false;
			break;
		}
		break;
	default:
		CharacterOwner->bUseControllerRotationYaw = bDefaultUseControllerRotationYaw;
		bOrientRotationToMovement = bDefaultOrientRotationToMovement;
	}

	switch (PreviousMovementMode)
	{
	case MOVE_Custom:
		switch (PreviousCustomMode)
		{
		// If we just finished wall running...
		case ECustomMovementMode::MOVE_WallRunning:
		// If we just finished climbing...
		case ECustomMovementMode::MOVE_Climbing:
		default:
			break;
		}
		break;
	default:
		break;
	}
	
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

// empty base implementation, intended for derived classes to override.
void UGACharacterMovementComponent::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);
}
#pragma endregion

#pragma region PhysicsCalculators
//void UGACharacterMovementComponent::StartNewPhysics(float DeltaTime, int32 Iterations)
//{
//	if ((deltaTime < MIN_TICK_TIME) || (Iterations >= MaxSimulationIterations) || !HasValidData())
//	{
//		return;
//	}
//
//	if (UpdatedComponent->IsSimulatingPhysics())
//	{
//		UE_LOG(LogCharacterMovement, Log, TEXT("UCharacterMovementComponent::StartNewPhysics: UpdateComponent (%s) is simulating physics - aborting."), *UpdatedComponent->GetPathName());
//		return;
//	}
//
//	const bool bSavedMovementInProgress = bMovementInProgress;
//	bMovementInProgress = true;
//
//	switch (MovementMode)
//	{
//	case MOVE_None:
//		break;
//	case MOVE_Walking:
//		PhysWalking(deltaTime, Iterations);
//		break;
//	case MOVE_NavWalking:
//		PhysNavWalking(deltaTime, Iterations);
//		break;
//	case MOVE_Falling:
//		PhysFalling(deltaTime, Iterations);
//		break;
//	case MOVE_Flying:
//		PhysFlying(deltaTime, Iterations);
//		break;
//	case MOVE_Swimming:
//		PhysSwimming(deltaTime, Iterations);
//		break;
//	case MOVE_Custom:
//		PhysCustom(deltaTime, Iterations);
//		break;
//	default:
//		UE_LOG(LogCharacterMovement, Warning, TEXT("%s has unsupported movement mode %d"), *CharacterOwner->GetName(), int32(MovementMode));
//		SetMovementMode(MOVE_None);
//		break;
//	}
//
//	bMovementInProgress = bSavedMovementInProgress;
//	if (bDeferUpdateMoveComponent)
//	{
//		SetUpdatedComponent(DeferredUpdatedMoveComponent);
//	}
//}

void UGACharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	// Phys functions should only run for characters with ROLE_Authority or ROLE_AutonumousProxy. However, Unreal calls PhysCustom in
	// two seperate locations, one of which doesn't check the role, so we must check it here to prevent this code from running on simulated proxies.
	if (GetOwner()->GetLocalRole() < ROLE_AutonomousProxy) return;

	switch (CustomMovementMode)
	{
	case ECustomMovementMode::MOVE_WallRunning:
		PhysWallRunning(DeltaTime, Iterations);
		break;
	case ECustomMovementMode::MOVE_Climbing:
		PhysClimbing(DeltaTime, Iterations);
		break;
	}

	// Not sure if needed
	Super::PhysCustom(DeltaTime, Iterations);
}

void UGACharacterMovementComponent::PhysWallRunning(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME) return;

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	float remainingTime = DeltaTime;

	// Perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		if (!HasValidData())
		{
			return;
		}

		++Iterations;

		// Time steps taken from delta time
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		WallRunDuration += timeTick;

		// Retain previous location value
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		// Make sure the required wall run keys are still down
		if (!bForwardKeysAreDown || bWantsToCrouch)
		{
			EndWallRun();
			StartNewPhysics(timeTick, --Iterations);
			return;
		}

		// Make sure we are next to a wall. Provide a vertical tolerance for the linetrace since it's possible the server has
		// moved our character slightly since we've begun the wall run. In the event we're right at the top/bottom of a wall. we need this
		// tolerance value so we don't Immediately fall off the wall
		if (!IsNextToWall(LineTraceVerticalTolerance))
		{
			EndWallRun();

			if (WallRunSide == ENavType::Front)
			{
				BeginClimb();
			}

			StartNewPhysics(timeTick, --Iterations);
			return;
		}

		// Apply acceleration
		CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration());
		// Save new Velocity
		const FVector MoveVelocity = Velocity;
		// Set the owning player's new velocity based on the wall run direction
		FVector newVelocity = WallRunDirection * MoveVelocity.Size();

		if (WallRunDuration >= MaxWallRunDuration)
		{
			// Compute current gravity
			const FVector Gravity = -GetPawnOwner()->GetGravityDirection() * GetGravityZ();
			float GravityTime = timeTick;

			// Apply gravity
			newVelocity = NewFallVelocity(newVelocity, Gravity, GravityTime);
		}

		const FVector Delta = newVelocity * DeltaTime;
		const bool bZeroDelta = Delta.IsNearlyZero();

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

			float subTimeTickRemaining = timeTick * (1.f - Hit.Time);

			// WE HAVE TO CHECK AND CALL ProcessLanded() OURSELVES!!!!
			// No wonder its not getting called on its own...
			if (Hit.bBlockingHit)
			{
				if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
				{
					remainingTime += subTimeTickRemaining;
					ProcessLanded(Hit, remainingTime, Iterations);
					return;
				}
				else 
				{
					EndWallRun();
					WallRunCheck(FVector::ZeroVector, Hit);
					ClimbCheck(FVector::ZeroVector, Hit);
				}
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			EndWallRun();
			StartNewPhysics(timeTick, Iterations);
			return;
		}
	}
}

void UGACharacterMovementComponent::PhysClimbing(float DeltaTime, int32 Iterations) 
{
	if (DeltaTime < MIN_TICK_TIME) return;

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	float remainingTime = DeltaTime;

	// Perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		++Iterations;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Retain previous location value
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		// Retain previous velocity value
		const FVector OldVelocity = Velocity;

		// Apply acceleration
		if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration());
		}

		if (bClimbingLedge)
		{
			ClimbLedge(timeTick, Iterations);
			return;
		}

		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FHitResult Hit;

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			MoveAlongWall(MoveVelocity, timeTick, &Hit);
		}

		// Update wall.
		// StepUp might have already done it for us.
		/*if (StepDownResult.bComputedWall)
		{
			CurrentWall = StepDownResult.WallResult;
		}
		else*/
		if (Hit.bBlockingHit)
		{
			float subTimeTickRemaining = timeTick * (1.f - Hit.Time);

			// WE HAVE TO CHECK AND CALL ProcessLanded() OURSELVES!!!!
			// No wonder its not getting called on its own...
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
			{
				remainingTime += subTimeTickRemaining;
				ProcessLanded(Hit, remainingTime, Iterations);
				return;
			}
		}
		else
		{
			// FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
			FindWall(UpdatedComponent->GetComponentLocation(), CurrentWall, NULL);

			// If trace is not hitting a wall anymore
			if (!CurrentWall.bBlockingHit)
			{
				// If we were traveling up a wall before reaching the top
				if (OldVelocity.Z > 0)
				{
					BeginClimbLedge();
					StartNewPhysics(remainingTime, Iterations);
					return;
				}

				EndClimb();
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
		}
		
		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}
}

void UGACharacterMovementComponent::MoveAlongWall(const FVector& InVelocity, float DeltaSeconds, FHitResult* OutHit)
{
	if (!CurrentWall.IsClimbableWall()) return;
	// UE_LOG(LogTemp, Warning, TEXT("moving along wall."));
	
	// Move along the current wall
	const FVector Delta = (CurrentWall.HitResult.ImpactNormal ^ FVector::UpVector).Normalize() * InVelocity * DeltaSeconds;
	FHitResult Hit(1.f);
	const FVector RampVector = ComputeWallMovementDelta(Delta, CurrentWall.HitResult, CurrentWall.bLineTrace);
	const FRotator Rotation = ComputeOrientRotationToWall(DeltaSeconds, CurrentWall.HitResult.ImpactNormal);
	SafeMoveUpdatedComponent(RampVector, Rotation, true, Hit);
	float LastMoveTimeSlice = DeltaSeconds;
	float subTimeTickRemaining = LastMoveTimeSlice * (1.f - Hit.Time);

	// Send movement result out
	*OutHit = Hit;
}

void UGACharacterMovementComponent::ClimbLedge(float DeltaTime, int32 Iterations)
{
	FHitResult traceHit;
	const FVector Forward = PawnOwner->GetActorForwardVector();
	const FVector start = UpdatedComponent->GetComponentLocation() + (-1 * Forward * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius());
	const FVector end = start + (-2.1 * FVector::UpVector * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	if (LineTrace(traceHit, start, end))
	{
		bClimbingLedge = false;
		EndClimb();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	FHitResult ForwardHit(1.0f);
	const FRotator Rotation = ComputeOrientRotationToWall(DeltaTime, CurrentWall.HitResult.ImpactNormal);
	SafeMoveUpdatedComponent(Forward * MaxClimbSpeed * DeltaTime, Rotation, true, ForwardHit);

	if (ForwardHit.bBlockingHit)
	{
		FHitResult UpHit(1.0f);
		SafeMoveUpdatedComponent(FVector::UpVector * MaxClimbSpeed * DeltaTime, Rotation, true, UpHit);

		if (UpHit.bBlockingHit)
		{
			return; // Something is preventing us from climbing upward.
		}
	}
}

void UGACharacterMovementComponent::ComputeWallDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindWallResult& OutWallResult, float SweepRadius, const FHitResult* ForwardSweepResult) const
{
	OutWallResult.Clear();
	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	bool bSkipSweep = false;
	if (ForwardSweepResult != NULL && ForwardSweepResult->IsValidBlockingHit())
	{
		// Only if the supplied sweep was horizontal and forward.
		if ((ForwardSweepResult->TraceStart.X <= ForwardSweepResult->TraceEnd.X) &&
			FVector2D(ForwardSweepResult->TraceStart.Y - ForwardSweepResult->TraceEnd.Y,
				ForwardSweepResult->TraceStart.Z - ForwardSweepResult->TraceEnd.Z).SizeSquared() <= KINDA_SMALL_NUMBER)
		{
			// Don't try a redundant sweep, regardless of whether this sweep is usable.
			bSkipSweep = true;

			const bool bIsClimbable = IsClimbable(*ForwardSweepResult);
			const float WallDist = (CapsuleLocation.X - ForwardSweepResult->Location.X);
			OutWallResult.SetFromSweep(*ForwardSweepResult, WallDist, bIsClimbable);

			if (bIsClimbable)
			{
				// Use the supplied forward sweep as the floor hit result.			
				return;
			}
		}
	}

	// We require the sweep distance to be >= the line distance, otherwise the HitResult can't be interpreted as the sweep result.
	if (SweepDistance < LineDistance)
	{
		ensure(SweepDistance >= LineDistance);
		return;
	}

	bool bBlockingHit = false;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ComputeWallDist), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	// Since we require a longer sweep than line trace, we don't want to run the line trace if the sweep missed everything.
	// We do however want to try a line trace if the sweep was stuck in penetration.
	if (false)// !OutWallResult.bBlockingHit && !OutWallResult.HitResult.bStartPenetrating)
	{
		OutWallResult.WallDist = SweepDistance;
		return;
	}
	
	// Line trace
	if (LineDistance > 0.f)
	{
		const float ShrinkRadius = PawnRadius;
		const FVector LineTraceStart = CapsuleLocation;
		const float TraceDist = LineDistance + ShrinkRadius;
		const FVector Front = CharacterOwner->GetActorForwardVector() * TraceDist;
		// QueryParams.TraceTag = SCENE_QUERY_STAT_NAME_ONLY(FloorLineTrace);

		FHitResult Hit(1.f);
		DrawDebugLine(GetWorld(), LineTraceStart, LineTraceStart + Front, FColor::Black);
		bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, LineTraceStart, LineTraceStart + Front, CollisionChannel, QueryParams, ResponseParam);

		if (bBlockingHit)
		{
			if (Hit.Time > 0.f)
			{
				// Reduce hit distance by ShrinkHeight because we started the trace higher than the base.
				// We allow negative distances here, because this allows us to pull out of penetrations.
				const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnHalfHeight);
				const float LineResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkRadius);

				OutWallResult.bBlockingHit = true;
				if (LineResult <= LineDistance && IsClimbable(Hit))
				{
					OutWallResult.SetFromLineTrace(Hit, LineResult, true);
					return;
				}
			}
		}
	}

	// No hits were acceptable.
	OutWallResult.bClimbableWall = false;
}

void UGACharacterMovementComponent::FindWall(const FVector& CapsuleLocation, FFindWallResult& OutWallResult, bool bCanUseCachedLocation, const FHitResult* ForwardSweepResult) const
{
	// No collision, no wall...
	if (!HasValidData() || !UpdatedComponent->IsQueryCollisionEnabled())
	{
		OutWallResult.Clear();
		return;
	}

	check(CharacterOwner->GetCapsuleComponent());

	// Increase height check slightly if walking, to prevent floor height adjustment from later invalidating the floor result.
	const float HeightCheckAdjust = (IsMovingOnWall() ? MAX_FLOOR_DIST + UE_KINDA_SMALL_NUMBER : -MAX_FLOOR_DIST);

	float WallSweepTraceDist = FMath::Max(MAX_FLOOR_DIST, MaxStepHeight + HeightCheckAdjust);
	float WallLineTraceDist = WallSweepTraceDist;

	ComputeWallDist(CapsuleLocation, WallLineTraceDist, WallSweepTraceDist, OutWallResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), ForwardSweepResult);
	/*

	FHitResult hitResult;
	auto lineTrace = [&](const FVector& start, const FVector& end)
	{
		return (LineTrace(hitResult, start, end));
	};

	if (lineTrace(CapsuleLocation, CapsuleLocation + (100.0f * PawnOwner->GetActorForwardVector())))
	{
		OutWallResult.SetFromLineTrace(hitResult, 2.0f, true);
	}
	else OutWallResult.bClimbableWall = false;

	*/
}

FVector UGACharacterMovementComponent::ComputeWallMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const
{
	// ComputeGroundMovementDelta()

	const FVector WallNormal = RampHit.ImpactNormal;
	const FVector ContactNormal = RampHit.Normal;

	if (WallNormal.X < (1.f - KINDA_SMALL_NUMBER) && WallNormal.X > KINDA_SMALL_NUMBER && ContactNormal.X > KINDA_SMALL_NUMBER && !bHitFromLineTrace && IsClimbable(RampHit))
	{
		// Compute a vector that moves parallel to the surface, by projecting the horizontal movement direction onto the ramp.
		const float WallDotDelta = (WallNormal | Delta);
		FVector RampMovement(-WallDotDelta / WallNormal.X, Delta.Y, Delta.Z);

		if (true)
			return RampMovement.GetSafeNormal() * Delta.Size();
		else
			return RampMovement;
	}

	return Delta;
}

FRotator UGACharacterMovementComponent::ComputeOrientRotationToWall(float DeltaTime, const FVector& SurfaceNormal) const
{
	FVector Forward = -SurfaceNormal;
	FVector Up = FVector::UpVector;
	FVector Right = Up ^ Forward;

	Forward.Normalize();
	Right.Normalize();
	Up.Normalize();

	const FMatrix RotMatrix(Forward, Right, Up, FVector::ZeroVector);

	const FRotator Rotation = RotMatrix.Rotator();

	const FRotator Current = UpdatedComponent->GetComponentRotation();
	return Current.Equals(Rotation) ? (Current + ((Rotation - Current).GetNormalized() * DeltaTime * 10)).GetNormalized() : Rotation;
}

FVector UGACharacterMovementComponent::CalculateFloorInfluence(const FVector& InSlope) const
{
	if (InSlope == FVector::UpVector)
	{
		return FVector::ZeroVector;
	}

	const FVector SlideDirection = FVector::CrossProduct(InSlope, FVector::CrossProduct(InSlope, FVector::UpVector));
	const float SlopeIntensity = FMath::Clamp<float>(1 - FVector::DotProduct(InSlope, FVector::UpVector), 0, 1);

	return SlideDirection * (SlideAcceleration * SlopeIntensity);
}
#pragma endregion

bool UGACharacterMovementComponent::IsSprinting() const
{
	AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner);
	return GACharacter && GACharacter->bIsSprinting;
}

bool UGACharacterMovementComponent::IsSliding() const
{
	AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner);
	return GACharacter && GACharacter->bIsSliding;
}

bool UGACharacterMovementComponent::IsMovingOnWall() const
{
	return (MovementMode == EMovementMode::MOVE_Custom) && ((CustomMovementMode == MOVE_Climbing) || (CustomMovementMode == MOVE_WallRunning)) && UpdatedComponent;
}

bool UGACharacterMovementComponent::IsWallRunning() const
{
	return IsCustomMovementMode(ECustomMovementMode::MOVE_WallRunning) && UpdatedComponent;
}

bool UGACharacterMovementComponent::IsClimbing() const
{
	return IsCustomMovementMode(ECustomMovementMode::MOVE_Climbing) && UpdatedComponent;
}

float UGACharacterMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
	{
		if (IsSliding())
		{
			return MaxSlideSpeed;
		}
		else if (IsCrouching())
		{
			return MaxWalkSpeedCrouched;
		}
		else if (IsSprinting())
		{
			return MaxSprintSpeed;
		}
		else
		{
			return MaxWalkSpeed;
		}
	}
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		switch (CustomMovementMode)
		{
		case MOVE_WallRunning:
			return MaxWallRunSpeed;
		case MOVE_Climbing:
			return MaxClimbSpeed;
		default:
			return MaxCustomMovementSpeed;
		}
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	};
}

float UGACharacterMovementComponent::GetMaxAcceleration() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
	{
		if (IsSprinting())
		{
			return SprintAcceleration;
		}
	}
	case MOVE_Custom:
		switch (CustomMovementMode)
		{
		case MOVE_WallRunning:
		default:
			return UCharacterMovementComponent::GetMaxAcceleration();
		}
	case MOVE_None:
	default:
		return UCharacterMovementComponent::GetMaxAcceleration();
	};

	return UCharacterMovementComponent::GetMaxAcceleration();
}

float UGACharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	// Super::GetMaxBrakingDeceleration()

	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		if (IsSliding()) return BrakingDecelerationSliding;
		else return BrakingDecelerationWalking;
	case MOVE_Falling:
		return BrakingDecelerationFalling;
	case MOVE_Swimming:
		return BrakingDecelerationSwimming;
	case MOVE_Flying:
		return BrakingDecelerationFlying;
	case MOVE_Custom:
		switch (CustomMovementMode)
		{
		case MOVE_WallRunning:
			return 1024.f;
		case MOVE_Climbing:
			return BrakingDecelerationClimbing;
		default:
			return 0.f;
		}
	case MOVE_None:
	default:
		return 0.f;
	}
}

#pragma region MovementRequests
bool UGACharacterMovementComponent::CanSlide() const
{
	AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner);
	return CanEverSlide() && IsMovingOnGround() && GACharacter && GACharacter->CanSlide() && GACharacter->bIsSprinting;
}

void UGACharacterMovementComponent::StartSliding()
{
	SetMovementMode(MOVE_Walking);

	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
	{
		if (!GACharacter->CanSlide()) return;
		GACharacter->OnBeginSlide();
	}

	bWantsToSlide = false;
	SlideConsecutiveResetDuration = 0.0f;
	++CurrentSlides;
	Velocity = CharacterOwner->GetActorForwardVector() * 
		(Velocity.Size() + 
		(MaxSprintSpeed *
		(bConsecutiveSlidesLoseMomentum ? (1.0f / CurrentSlides) : 1)));
	// GroundFriction = SlideGroundFriction;
	// bUseSeparateBrakingFriction = false;
	bCanWalkOffLedgesWhenCrouching = true;
}

void UGACharacterMovementComponent::StopSliding()
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
	{
		GACharacter->OnEndSlide();
	}

	bWantsToSlide = false;
	GroundFriction = DefaultGroundFriction;
	// bUseSeparateBrakingFriction = true;
	bCanWalkOffLedgesWhenCrouching = false;
}

void UGACharacterMovementComponent::ResetSliding()
{
	CurrentSlides = 0;
}

void UGACharacterMovementComponent::StartSprinting()
{
	bSprintKeyDown = true;
}

void UGACharacterMovementComponent::StopSprinting()
{
	bSprintKeyDown = false;
}

bool UGACharacterMovementComponent::BeginWallRun()
{
	if (bForwardKeysAreDown && Velocity.Size() >= MaxWalkSpeed)
	{
		// Convenience if statement for locally checked GACharacter cast
		if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
			if (GACharacter->CanWallRun())
			{
				if (GACharacter->bIsSliding)
					StopSliding();
				GACharacter->OnBeginWallRun();
			}
			else return false;

		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_WallRunning);
		WallRunDuration = 0.0f;
		return true;
	}

	return false;
}

void UGACharacterMovementComponent::EndWallRun()
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner)) GACharacter->OnEndWallRun();

	// Set the movement mode back to falling
	SetMovementMode(EMovementMode::MOVE_Falling);
	WallRunDuration = 0.0f;
}

bool UGACharacterMovementComponent::BeginClimb()
{	
	if (bForwardKeysAreDown)
	{
		// Convenience if statement for locally checked GACharacter cast
		if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
			if (!GACharacter->CanClimb())
				return false;

		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_Climbing);
		return true;
	}

	return false;
}

void UGACharacterMovementComponent::BeginClimbLedge()
{
	bClimbingLedge = true;
}

void UGACharacterMovementComponent::EndClimb()
{
	SetMovementMode(EMovementMode::MOVE_Falling);
}
#pragma endregion

bool UGACharacterMovementComponent::AreRequiredKeysDown() const
{
	// Since this function is checking for input, it should only be called for locally controlled character
	if (!GetPawnOwner()->IsLocallyControlled())
		return false;

	// Make sure the spring key is down (the player may only wall run if he is holding toward wall and forward)
	TArray<FInputAxisKeyMapping> MoveForwardKeyMappings;
	UInputSettings::GetInputSettings()->GetAxisMappingByName("MoveForward", MoveForwardKeyMappings);
	for (FInputAxisKeyMapping& MoveForwardKeyMapping : MoveForwardKeyMappings)
	{
		if (const auto* Controller = GetPawnOwner()->GetController<APlayerController>())
		{
			if (Controller->IsInputKeyDown(MoveForwardKeyMapping.Key))
			{
				return true;
			}
		}
	}

	return false;
}

bool UGACharacterMovementComponent::IsClimbable(const FHitResult& Hit) const
{
	if (!Hit.IsValidBlockingHit())
	{
		// No hit, or starting in penetration
		return false;
	}

	// CLimb up only vertical surfaces.
	if (Hit.ImpactNormal.Z >= KINDA_SMALL_NUMBER)
	{
		return false;
	}

	// float TestClimableX = GetClimbableWallX();
	float TestWalkableZ = GetWalkableFloorZ();

	// See if this component overrides the walkable floor z.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (HitComponent)
	{
		const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
		TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
	}

	// Can't climb on this surface unless it is too steep.
	if (Hit.ImpactNormal.Z >= TestWalkableZ)
	{
		return false;
	}

	return true;
}

void UGACharacterMovementComponent::FindWallRunDirectionAndSide(const FVector& surface_normal, FVector& direction, ENavType& side) const
{
	FVector CrossVector;
	double WallAngle = FVector2D::DotProduct(FVector2D(surface_normal), FVector2D(GetPawnOwner()->GetActorRightVector()));

	if (WallAngle <= ForwardWallRunThreshold && WallAngle >= -ForwardWallRunThreshold)
	{
		side = ENavType::Front;
		direction = FVector::UpVector;
		return;
	}
	else if (WallAngle > 0.0)
	{
		side = ENavType::Right;
		CrossVector = FVector(0.0f, 0.0f, 1.0f);
	}
	else
	{
		side = ENavType::Left;
		CrossVector = FVector(0.0f, 0.0f, -1.0f);
	}

	// Find the direction parallel to the wall in the direction the player is moving
	direction = FVector::CrossProduct(surface_normal, CrossVector);
}

bool UGACharacterMovementComponent::CanSurfaceBeWallRan(const FVector& surface_normal) const
{
	// Return false if the surface normal is facing down
	if (surface_normal.Z < -0.05f) return false;

	// Surface normal without Z
	FVector normalNoZ = FVector(surface_normal.X, surface_normal.Y, 0.0f);
	normalNoZ.Normalize();

	// Find the angle of the wall
	float wallAngle = FMath::Acos(FVector::DotProduct(normalNoZ, surface_normal));

	// Return false if the surface normal is facing up
	return wallAngle < GetWalkableFloorAngle();
}

bool UGACharacterMovementComponent::IsCustomMovementMode(uint8 custom_movement_mode) const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == custom_movement_mode;
}

#pragma region CollisionChecks
bool UGACharacterMovementComponent::LineTrace(FHitResult& Hit, const FVector& start, const FVector& end) const
{
	bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, start, end, ECollisionChannel::ECC_Visibility);
	FColor Color = bBlockingHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), start, end, Color);
	if (bBlockingHit) DrawDebugPoint(GetWorld(), Hit.Location, 10.0f, Color);
	return bBlockingHit;
}

bool UGACharacterMovementComponent::SweepTrace(FHitResult& Hit, const FVector& start, const FVector& end) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(UpdatedPrimitive->GetOwner());
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	// Use a smaller radius to not get caught on walls
	// This also allows us to adjust out of penetrations.
	float PawnRadius, PawnHalfHeight;
	UpdatedPrimitive->CalcBoundingCylinder(PawnRadius, PawnHalfHeight);

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(PawnRadius * 0.5, PawnHalfHeight);
	bool bBlockingHit = GetWorld()->SweepSingleByChannel(Hit, start, end, FQuat::Identity, CollisionChannel, CapsuleShape, QueryParams, ResponseParam);
	FColor Color = bBlockingHit ? FColor::Green : FColor::Red;
	DrawDebugCapsule(GetWorld(), end, PawnHalfHeight, PawnRadius * 0.5, FQuat::Identity, Color);
	return bBlockingHit;
}

void UGACharacterMovementComponent::OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->GetRootComponent() && OtherActor->GetRootComponent()->GetCollisionObjectType() == ECC_Pawn) return;

	// Section
	{
		WallRunCheck(NormalImpulse, Hit);
		ClimbCheck(NormalImpulse, Hit);
	}
}

void UGACharacterMovementComponent::WallRunCheck(const FVector& NormalImpulse, const FHitResult& Hit)
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
		if (!GACharacter->CanWallRun())
			return;
	if (!CanEverWallRun()) return;
	if (IsMovingOnWall()) return;
	if (!CanSurfaceBeWallRan(Hit.ImpactNormal)) return;
	FindWallRunDirectionAndSide(Hit.ImpactNormal, WallRunDirection, WallRunSide);
	if (IsFalling())
	{
		if (WallRunSide == ENavType::Front) return;
	}
	else
	{
		if (WallRunSide != ENavType::Front) return;
	}
	if (!IsNextToWall()) return;

	BeginWallRun();
}

void UGACharacterMovementComponent::ClimbCheck(const FVector& NormalImpulse, const FHitResult& Hit)
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
		if (!GACharacter->CanClimb())
			return;
	if (!CanEverClimb()) return;
	if (IsMovingOnWall() || !IsFalling() || IsCrouching()) return;
	if (!IsInFrontOfWall()) return;
	WallRunDirection = FVector::UpVector;
	WallRunSide = ENavType::Front;

	BeginClimb();
}

bool UGACharacterMovementComponent::IsInFrontOfWall(float vertical_tolerance)
{
	const FVector traceStart = GetPawnOwner()->GetActorLocation() + (WallRunDirection * 20.0f);
	const FVector traceEnd = traceStart + (GetPawnOwner()->GetActorForwardVector() * 50);
	FHitResult hitResult;

	// Create a helper lambda for performing the line trace
	auto lineTrace = [&](const FVector& start, const FVector& end)
	{
		return (LineTrace(hitResult, start, end));
	};

	// If a vertical tolerance was provided we want to do two line traces - one above and one below the calculated line
	if (vertical_tolerance > FLT_EPSILON)
	{
		// If both line traces miss the wall then return false, we're not next to a wall
		if (lineTrace(FVector(traceStart.X, traceStart.Y, traceStart.Z + vertical_tolerance / 2.0f), FVector(traceEnd.X, traceEnd.Y, traceEnd.Z + vertical_tolerance / 2.0f)) == false &&
			lineTrace(FVector(traceStart.X, traceStart.Y, traceStart.Z - vertical_tolerance / 2.0f), FVector(traceEnd.X, traceEnd.Y, traceEnd.Z - vertical_tolerance / 2.0f)) == false)
		{
			return false;
		}
	}
	// If no vertical tolerance was provided we just want to do one line trace using the caclulated line
	else
	{
		// return false if the line trace misses the wall
		if (lineTrace(traceStart, traceEnd) == false)
			return false;
	}

	// Make sure we're still on the side of the wall we expect to be on
	ENavType newWallRunSide;
	FindWallRunDirectionAndSide(hitResult.ImpactNormal, WallRunDirection, newWallRunSide);
	if (newWallRunSide != WallRunSide)
	{
		return false;
	}

	return true;
}

bool UGACharacterMovementComponent::IsNextToWall(float vertical_tolerance)
{
	// Do a line trace from the player into the wall to make sure we're stil along the side of a wall
	FVector crossVector = FVector();
	switch (WallRunSide)
	{
	case ENavType::Front:
		return IsInFrontOfWall(vertical_tolerance);
	case ENavType::Left:
		crossVector = FVector(0.0f, 0.0f, -1.0f);
		break;
	case ENavType::Right:
		crossVector = FVector(0.0f, 0.0f, 1.0f);
		break;
	}
	const FVector traceStart = GetPawnOwner()->GetActorLocation() + (WallRunDirection * 20.0f);
	const FVector traceEnd = traceStart + (FVector::CrossProduct(WallRunDirection, crossVector) * 70);
	FHitResult hitResult;

	// Create a helper lambda for performing the line trace
	auto lineTrace = [&](const FVector& start, const FVector& end)
	{
		return (LineTrace(hitResult, start, end));
	};

	// If a vertical tolerance was provided we want to do two line traces - one above and one below the calculated line
	if (vertical_tolerance > FLT_EPSILON)
	{
		// If both line traces miss the wall then return false, we're not next to a wall
		if (lineTrace(FVector(traceStart.X, traceStart.Y, traceStart.Z + vertical_tolerance / 2.0f), FVector(traceEnd.X, traceEnd.Y, traceEnd.Z + vertical_tolerance / 2.0f)) == false &&
			lineTrace(FVector(traceStart.X, traceStart.Y, traceStart.Z - vertical_tolerance / 2.0f), FVector(traceEnd.X, traceEnd.Y, traceEnd.Z - vertical_tolerance / 2.0f)) == false)
		{
			return false;
		}
	}
	// If no vertical tolerance was provided we just want to do one line trace using the caclulated line
	else
	{
		// return false if the line trace misses the wall
		if (lineTrace(traceStart, traceEnd) == false)
			return false;
	}

	// Make sure we're still on the side of the wall we expect to be on
	ENavType newWallRunSide;
	FindWallRunDirectionAndSide(hitResult.ImpactNormal, WallRunDirection, newWallRunSide);
	if (newWallRunSide != WallRunSide)
	{
		return false;
	}

	return true;
}
#pragma endregion

#pragma region JumpOverrides
bool UGACharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling() || IsMovingOnWall()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

bool UGACharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
		{
			if (IsWallRunning())
			{
				FVector Axis;
				switch (WallRunSide)
				{
				case ENavType::Front:
					Velocity = ((FVector::ZeroVector - PawnOwner->GetActorForwardVector()) + FVector::UpVector) * JumpZVelocity;
					EndWallRun();
					SetMovementMode(MOVE_Falling);
					return true;
				case ENavType::Left:
					Axis = FVector(0, 0, 1);
					break;
				case ENavType::Right:
					Axis = FVector(0, 0, -1);
					break;
				}

				Velocity += (WallRunDirection.Cross(Axis) + FVector::UpVector) * JumpZVelocity;
				EndWallRun();
				SetMovementMode(MOVE_Falling);
				return true;
			}

			if (IsClimbing())
			{
				Velocity += ((FVector::ZeroVector - PawnOwner->GetActorForwardVector()) + FVector::UpVector) * JumpZVelocity;
				EndClimb();
				SetMovementMode(MOVE_Falling);
				return true;
			}
		}
	}

	return Super::DoJump(bReplayingMoves);
}

void UGACharacterMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	// If we landed while wall running, make sure we stop wall running
	if (IsWallRunning()) EndWallRun();

	// if we landed while climbing, make sure we stop climbing
	else if (IsClimbing()) EndClimb();

	// If we land and want to slide, then we slide. Otherwise return to walking
	if (bWantsToSlide) StartSliding();
	
	SetMovementMode(MOVE_Walking);

	Super::ProcessLanded(Hit, remainingTime, Iterations);
}
#pragma endregion

#pragma region CrouchOverrides
void UGACharacterMovementComponent::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);

	// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
	const bool bIsCrouching = IsCrouching();
	if (bIsCrouching)
	{
		if (CanSlide())
		{
			StartSliding();
		}
		else if (IsFalling())
		{
			bWantsToSlide = true;
		}
	}
}

void UGACharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	bWantsToSlide = false;

	if (IsMovingOnGround())
	{
		if (Velocity.Size() <= MaxWalkSpeed)
		{
			Super::UnCrouch(bClientSimulation);
		}
	}
	else
	{
		Super::UnCrouch(bClientSimulation);
	}

	// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
	const bool bIsCrouching = IsCrouching();
	if (!bIsCrouching && IsSliding())
	{
		StopSliding();
	}
}
#pragma endregion

#pragma region Non-MovementComponentClasses
void FFindWallResult::SetFromSweep(const FHitResult& InHit, const float InSweepWallDist, const bool bIsClimbableWall)
{
	bBlockingHit = InHit.IsValidBlockingHit();
	bClimbableWall = bIsClimbableWall;
	bLineTrace = false;
	WallDist = InSweepWallDist;
	LineDist = 0.f;
	HitResult = InHit;
}

void FFindWallResult::SetFromLineTrace(const FHitResult& InHit, const float InLineDist, const bool bIsClimbableWall)
{
	bBlockingHit = InHit.IsValidBlockingHit();
	bClimbableWall = bIsClimbableWall;
	bLineTrace = false;
	WallDist = InLineDist;
	LineDist = 0.f;
	HitResult = InHit;
}

void FSavedMove_GA::Clear()
{
	Super::Clear();

	// Clear all values
	SavedWantsToSprint = 0;
	SavedWallRunKeysDown = 0;
}

uint8 FSavedMove_GA::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	/* There are 4 custom move flags for us to use. Below is what each is currently being used for:
	FLAG_Custom_0		= 0x10, // Sprinting
	FLAG_Custom_1		= 0x20, // WallRunning
	FLAG_Custom_2		= 0x40, // Climbing
	FLAG_Custom_3		= 0x80, // Unused
	*/

	// Write to the compressed flags
	if (SavedWantsToSprint)
		Result |= FLAG_Custom_0;
	if (SavedWallRunKeysDown)
		Result |= FLAG_Custom_1;

	return Result;
}

bool FSavedMove_GA::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_GA* NewMove = static_cast<const FSavedMove_GA*>(NewMovePtr.Get());

	// As an optimization, check if the engine can combine saved moves.
	if (SavedWantsToSprint != NewMove->SavedWantsToSprint ||
		SavedWallRunKeysDown != NewMove->SavedWallRunKeysDown)
	{
		return false;
	}

	return Super::CanCombineWith(NewMovePtr, Character, MaxDelta);
}

void FSavedMove_GA::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UGACharacterMovementComponent* charMov = Cast<UGACharacterMovementComponent>(Character->GetCharacterMovement());
	if (charMov)
	{
		// Copy Values into the saved move
		SavedWantsToSprint = charMov->bWantsToSprint;
		SavedWallRunKeysDown = charMov->bForwardKeysAreDown;
	}
}

void FSavedMove_GA::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UGACharacterMovementComponent* charMov = Cast<UGACharacterMovementComponent>(Character->GetCharacterMovement());
	if (charMov)
	{
		// Copy Values into the saved move
		charMov->bWantsToSprint = SavedWantsToSprint;
		charMov->bForwardKeysAreDown = SavedWallRunKeysDown;
	}
}

FNetworkPredictionData_Client_GA::FNetworkPredictionData_Client_GA(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_GA::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_GA());
}
#pragma endregion
