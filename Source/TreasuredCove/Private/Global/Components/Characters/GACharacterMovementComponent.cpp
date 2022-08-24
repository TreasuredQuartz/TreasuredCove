// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/GACharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GACharacter.h"
#include "CustomMovementMode.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"

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
	if (GetPawnOwner()->IsLocallyControlled())
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

		// Update if required keys are down
		bForwardKeysAreDown = AreRequiredKeysDown();
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
	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
		case ECustomMovementMode::MOVE_WallRunning:
			// Stop current Movement
			StopMovementImmediately();
			break;
		case ECustomMovementMode::MOVE_Climbing:
			// UE_LOG(LogTemp, Warning, TEXT("Custom Movement Mode changed to Climbing."));
			// Stop current Movement
			StopMovementImmediately();
			bPrevOrientRotationToMovement = bOrientRotationToMovement;
			bOrientRotationToMovement = false;
			break;
		}
	}

	if (PreviousMovementMode == MOVE_Custom)
	{
		switch (PreviousCustomMode)
		{
		// If we just finished wall running...
		case ECustomMovementMode::MOVE_WallRunning:
			break;
		case ECustomMovementMode::MOVE_Climbing:
			bOrientRotationToMovement = bPrevOrientRotationToMovement;
			break;
		}
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

// empty base implementation, intended for derived classes to override.
void UGACharacterMovementComponent::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{

}
#pragma endregion

#pragma region PhysicsCalculators
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

	++Iterations;
	WallRunDuration += DeltaTime;

	// Make sure the required wall run keys are still down
	if (!bForwardKeysAreDown)
	{
		EndWallRun();
		return;
	}

	// Make sure we are next to a wall. Provide a vertical tolerance for the linetrace since it's possible the server has
	// moved our character slightly since we've begun the wall run. In the event we're right at the top/bottom of a wall. we need this
	// tolerance value so we don't Immediately fall off the wall
	if (!IsNextToWall(LineTraceVerticalTolerance))
	{
		EndWallRun();
		return;
	}

	// Set the owning player's new velocity based on the wall run direction
	FVector newVelocity;

	// Time steps taken from delta time
	float remainingTime = DeltaTime;
	float timeTick = GetSimulationTimeStep(remainingTime, Iterations);

	if (WallRunDuration < MaxWallRunDuration) 
	{
		newVelocity = WallRunDirection * MaxWallRunSpeed;
	}
	else
	{
		if (WallRunSide == ENavType::Front)
		{
			EndWallRun();
			BeginClimb();
			return;
		}

		// Compute current gravity
		const FVector Gravity = -GetPawnOwner()->GetGravityDirection() * GetGravityZ();
		float GravityTime = timeTick;

		newVelocity = WallRunDirection;
		newVelocity.X *= MaxWallRunSpeed;
		newVelocity.Y *= MaxWallRunSpeed;
		newVelocity.Z = Velocity.Z;

		// Apply gravity
		newVelocity = NewFallVelocity(newVelocity, Gravity, GravityTime);
	}

	Velocity = newVelocity;
	const FVector Adjusted = Velocity * DeltaTime;
	FHitResult Hit(1.f);

	SafeMoveUpdatedComponent(Adjusted, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (!HasValidData())
	{
		return;
	}

	float subTimeTickRemaining = timeTick * (1.f - Hit.Time);

	// WE HAVE TO CHECK AND CALL ProcessLanded() OURSELVES!!!!
	// No wonder its not getting called on its own...
	if ( Hit.bBlockingHit )
	{
		if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), Hit))
		{
			remainingTime += subTimeTickRemaining;
			ProcessLanded(Hit, remainingTime, Iterations);
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

	++Iterations;

	if (bClimbingLedge)
	{
		ClimbLedge(DeltaTime, Iterations);
		return;
	}

	// Compute move parameters
	FindWall(UpdatedComponent->GetComponentLocation(), CurrentWall, NULL);
	MoveAlongWall(DeltaTime, Iterations);
}

void UGACharacterMovementComponent::MoveAlongWall(float DeltaTime, int32 Iterations)
{
	if (!CurrentWall.IsClimbableWall()) return;
	// UE_LOG(LogTemp, Warning, TEXT("moving along wall."));

	const FVector LastInputVector = GetLastInputVector().GetSafeNormal();
	// Our input is zero, we don't move. No Momentum etc.
	if (LastInputVector.IsNearlyZero()) return;

	const FVector HorizontalDirection = ((FVector::UpVector ^ -CurrentWall.HitResult.ImpactNormal) * LastInputVector).GetSafeNormal();
	FVector TraceLocation;
	FVector TraceNormal;
	FHitResult traceHit;
	FHitResult hitResult;
	const float PawnRadius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();

	if (SweepTrace(hitResult, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentLocation() + (LastInputVector * (PawnRadius + 20))))
	{
		if (LastInputVector.Z < -SMALL_NUMBER)
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(hitResult.Location - UpdatedComponent->GetComponentLocation().GetSafeNormal() * MaxClimbSpeed, UpdatedComponent->GetComponentQuat(), true, Hit);

			if (Hit.bBlockingHit) 
			{
				float remainingTime = DeltaTime;
				const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
				remainingTime -= timeTick;
				ProcessLanded(Hit, remainingTime, Iterations);
				return;
			}
		}

		return;
		TraceLocation = hitResult.Location;
		TraceNormal = hitResult.ImpactNormal;
	}
	else if (!hitResult.bBlockingHit)
	{
		if (LineTrace(traceHit, hitResult.TraceEnd, hitResult.TraceEnd + (PawnOwner->GetActorForwardVector() * (PawnRadius + 20))))
		{
			TraceLocation = traceHit.Location;
			TraceNormal = traceHit.ImpactNormal;
		}
		else
		{
			if (LastInputVector.Z > 0)
			{
				BeginClimbLedge();
			}
		}
	}

	const FVector DesiredDirection = ((TraceLocation + TraceNormal * (PawnRadius + 5)) - UpdatedComponent->GetComponentLocation()).GetSafeNormal();
	const FVector newVelocity = DesiredDirection * MaxClimbSpeed;

	FHitResult Hit(1.f);
	const FVector Adjusted = newVelocity * DeltaTime;
	const FRotator Rotation = ComputeOrientRotationToWall(DeltaTime, TraceNormal);
	if (SafeMoveUpdatedComponent(Adjusted, Rotation.Quaternion(), true, Hit))
	{
		// UE_LOG(LogTemp, Warning, TEXT("Moving?"));
	}
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
		return;
	}

	FHitResult ForwardHit(1.0f);
	SafeMoveUpdatedComponent(Forward * MaxClimbSpeed * DeltaTime, UpdatedComponent->GetComponentQuat(), true, ForwardHit);

	if (ForwardHit.bBlockingHit)
	{
		FHitResult UpHit(1.0f);
		SafeMoveUpdatedComponent(FVector::UpVector * MaxClimbSpeed * DeltaTime, UpdatedComponent->GetComponentQuat(), true, UpHit);
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
	if (!OutWallResult.bBlockingHit && !OutWallResult.HitResult.bStartPenetrating)
	{
		OutWallResult.WallDist = SweepDistance;
		return;
	}
	
	// Line trace
	if (LineDistance > 0.f)
	{
		const float ShrinkHeight = PawnHalfHeight;
		const FVector LineTraceStart = CapsuleLocation;
		const float TraceDist = LineDistance + ShrinkHeight;
		const FVector Front = FVector(TraceDist, 0.f, 0.f);
		QueryParams.TraceTag = SCENE_QUERY_STAT_NAME_ONLY(FloorLineTrace);

		FHitResult Hit(1.f);
		bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, LineTraceStart, LineTraceStart + Front, CollisionChannel, QueryParams, ResponseParam);

		if (bBlockingHit)
		{
			if (Hit.Time > 0.f)
			{
				// Reduce hit distance by ShrinkHeight because we started the trace higher than the base.
				// We allow negative distances here, because this allows us to pull out of penetrations.
				const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnRadius);
				const float LineResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

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

	FHitResult hitResult;
	auto lineTrace = [&](const FVector& start, const FVector& end)
	{
		return (LineTrace(hitResult, start, end));
	};

	if (lineTrace(CapsuleLocation, CapsuleLocation + (100.0f * PawnOwner->GetActorForwardVector())))
	{
		OutWallResult.SetFromLineTrace(hitResult, 2.0f, true);
	}
}

FVector UGACharacterMovementComponent::ComputeWallMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const
{
	const FVector WallNormal = RampHit.ImpactNormal;
	const FVector ContactNormal = RampHit.Normal;

	if (WallNormal.X < (1.f - KINDA_SMALL_NUMBER) && WallNormal.X > KINDA_SMALL_NUMBER && ContactNormal.X > KINDA_SMALL_NUMBER && !bHitFromLineTrace && IsClimbable(RampHit))
	{
		// Compute a vector that moves parallel to the surface, by projecting the horizontal movement direction onto the ramp.
		const float WallDotDelta = (WallNormal | Delta);
		return FVector(-WallDotDelta / WallNormal.X, Delta.Y, Delta.Z);
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
	return Current != Rotation ? (Current + ((Rotation - Current).GetNormalized() * DeltaTime)).GetNormalized() : Current;
}
#pragma endregion

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
		if (IsCrouching())
		{
			return MaxWalkSpeedCrouched;
		}
		else
		{
			if (bWantsToSprint)
			{
				return MaxSprintSpeed;
			}

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
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	};
}

float UGACharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsMovingOnGround() && bWantsToSprint)
	{
		return SprintAcceleration;
	}

	return UCharacterMovementComponent::GetMaxAcceleration();
}

#pragma region MovementRequests
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
	if (bForwardKeysAreDown)
	{
		// Convenience if statement for locally checked GACharacter cast
		if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
			if (!GACharacter->CanWallRun())
				return false;

		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_WallRunning);
		WallRunDuration = 0.0f;
		return true;
	}

	return false;
}

void UGACharacterMovementComponent::EndWallRun()
{
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
	WallRunCheck(NormalImpulse, Hit);
	ClimbCheck(NormalImpulse, Hit);
}

void UGACharacterMovementComponent::WallRunCheck(const FVector& NormalImpulse, const FHitResult& Hit)
{
	if (IsClimbing()) return;
	if (IsWallRunning()) return;
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
	if (IsWallRunning()) return;
	if (IsClimbing()) return;
	if (!IsFalling()) return;
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
	const FVector traceEnd = traceStart + (FVector::CrossProduct(WallRunDirection, crossVector) * 50);
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
		(IsMovingOnGround() || IsFalling() || IsWallRunning() || IsClimbing()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

bool UGACharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		if (IsWallRunning())
		{
			FVector Axis;
			switch (WallRunSide) 
			{
			case ENavType::Front:
				Velocity = ((FVector::ZeroVector - PawnOwner->GetActorForwardVector()) + FVector::UpVector) * JumpZVelocity;
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
			SetMovementMode(MOVE_Falling);
			return true;
		}

		if (IsClimbing())
		{
			Velocity += ((FVector::ZeroVector - PawnOwner->GetActorForwardVector()) + FVector::UpVector) * JumpZVelocity;
			SetMovementMode(MOVE_Falling);
			return true;
		}

		if (IsCrouching())
		{
			Velocity += (FVector::UpVector * JumpZVelocity * 10);
			SetMovementMode(MOVE_Falling);
			return true;
		}
	}

	

	return Super::DoJump(bReplayingMoves);
}

void UGACharacterMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	// If we landed while wall running, make sure we stop wall running
	if (IsWallRunning()) EndWallRun();
	else if (IsClimbing()) EndClimb();
	SetMovementMode(MOVE_Walking);

	Super::ProcessLanded(Hit, remainingTime, Iterations);
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
