// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/GACharacterMovementComponent.h"
#include "GACharacter.h"
#include "CustomMovementMode.h"
#include "Components/CapsuleComponent.h"

// Framework
#include "GameFramework/Character.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PhysicsVolume.h"

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
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	
	// Local Machine's Tick portion...
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
			if (!IsSliding() && bWantsToSlide && IsMovingOnGround())
			{
				StartSliding();
			}
		}

		// Update if required keys are down
		bForwardKeysAreDown = AreRequiredKeysDown();
	}

	// Slide checks...
	if (CanEverSlide())
	{
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

	// Update Client from Server...
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
	{
		GACharacter->bIsSprinting = bWantsToSprint || IsSliding();
	}
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
			break;
			// If we just finished sliding...
		case ECustomMovementMode::MOVE_Sliding:
			break;
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

	float SubmergenceRatio = GetSubmergenceRatio();
	if (SubmergenceRatio >= 0.9)
	{
		UE_LOG(LogTemp, Warning, TEXT("Begin Swimming!"));
		if (bIsWading)
		{
			bIsWading = false;
			GetPhysicsVolume()->bWaterVolume = true;
			SetMovementMode(MOVE_Swimming);
		}
	}
	else if (IsSwimming())
	{
		// Reached Surface
		GetPhysicsVolume()->bWaterVolume = false;
		bIsWading = true;
		UE_LOG(LogTemp, Warning, TEXT("Stop Swimming!"));
	}
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

bool UGACharacterMovementComponent::PhysValidator(float remainingTime, int32 Iterations)
{
	return (remainingTime >= MIN_TICK_TIME) &&
		(Iterations < MaxSimulationIterations) &&
		HasValidData() &&
		(CharacterOwner->Controller ||
			bRunPhysicsWithNoController ||
			HasAnimRootMotion() ||
			CurrentRootMotion.HasOverrideVelocity() ||
			(CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy));
}

void UGACharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	// Phys functions should only run for characters with ROLE_Authority or ROLE_AutonumousProxy. However, Unreal calls PhysCustom in
	// two seperate locations, one of which doesn't check the role, so we must check it here to prevent this code from running on simulated proxies.
	if (GetOwner()->GetLocalRole() < ROLE_AutonomousProxy) return;

	switch (CustomMovementMode)
	{
	case ECustomMovementMode::MOVE_Sliding:
		PhysSliding(DeltaTime, Iterations);
		break;
	case ECustomMovementMode::MOVE_WallRunning:
		PhysWallRunning(DeltaTime, Iterations);
		break;
	case ECustomMovementMode::MOVE_Climbing:
		PhysClimbing(DeltaTime, Iterations);
		break;
	case ECustomMovementMode::MOVE_Vaulting:
		PhysVaulting(DeltaTime, Iterations);
		break;
	}

	// Not sure if needed
	Super::PhysCustom(DeltaTime, Iterations);
}

void UGACharacterMovementComponent::PhysSliding(float DeltaTime, int32 Iterations)
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
	while (PhysValidator(remainingTime, Iterations))
	{
		++Iterations;
		bJustTeleported = false; 
		bool bCheckedFall = false;
		bool bTriedLedgeMove = false;

		// Time steps taken from delta time
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		/* Save current values */

		// Old Floor Component We were sliding on.
		UPrimitiveComponent* const OldBase = GetMovementBase();
		// Last location of the component we were sliding on.
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		// Last location we physically were.
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		// Last Floor we were sliding on.
		const FFindFloorResult OldFloor = CurrentFloor;
		// Velocity before updating this frame/step.
		const FVector OldVelocity = Velocity;

		const bool bIsCrouching = IsCrouching();
		if (!bIsCrouching)
		{
			if (bWantsToSprint && Velocity.Size() <= MaxSprintSpeed)
			{
				StopSliding();
				SetMovementMode(MOVE_Walking);
				StartNewPhysics(timeTick, Iterations);
				return;
			}

			if (Velocity.Size() <= MaxWalkSpeed)
			{
				StopSliding();
				SetMovementMode(MOVE_Walking);
				StartNewPhysics(timeTick, Iterations);
				return;
			}
		}
		else if (Velocity.Size() <= MaxWalkSpeedCrouched)
		{
			StopSliding();
			SetMovementMode(MOVE_Walking);
			StartNewPhysics(timeTick, Iterations);
			return;
		}

		// Limit Acceleration to only left and right
		if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > 0.5)
		{
			// Acceleration = FVector::ZeroVector; // Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
		}
		else
		{
			// Acceleration = FVector::ZeroVector;
		}

		// Acceleration = FVector::ZeroVector;
		// Calc Velocity
		CalcVelocity(timeTick, SlideGroundFriction, false, BrakingDecelerationSliding);
		// Save new Velocity
		const FVector MoveVelocity = Velocity;
		// Floor Influence
		const FVector Force = CalculateFloorInfluence(CurrentFloor.HitResult.Normal);
		const FVector NewVelocity = MoveVelocity + Force;
		
		// Compute current gravity
		const FVector Gravity = -GetPawnOwner()->GetGravityDirection() * GetGravityZ() * SlideGravityMultiplier;
		float GravityTime = timeTick;
		// Apply gravity
		const FVector FallVelocity = NewFallVelocity(NewVelocity, Gravity, GravityTime);

		const FVector Delta = FallVelocity * timeTick;
		// const FVector VelPlaneDir = FVector::VectorPlaneProject(NewVelocity, CurrentFloor.HitResult.Normal).GetUnsafeNormal();
		const FQuat Rotation = UpdatedComponent->GetComponentQuat();// FRotationMatrix::MakeFromXZ(VelPlaneDir, CurrentFloor.HitResult.Normal).ToQuat();
		const bool bZeroDelta = Delta.IsNearlyZero();

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Delta, Rotation, true, Hit);
			
			if (Hit.Time < 1.f)
			{
				HandleImpact(Hit, DeltaTime, Delta);
				SlideAlongSurface(Delta, (1.f - Hit.Time), Hit.Normal, Hit, true);
			}

			if (IsFalling())
			{
				// pawn decided to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > UE_KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}
				StopSliding();
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				StopSliding();
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);

		if (!CurrentFloor.IsWalkableFloor()) // Slid off ledge
		{
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					StopSliding();
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						StopSliding();
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if (IsSwimming())
			{
				StopSliding();
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					StopSliding();
					return;
				}
				bCheckedFall = true;
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			Velocity = FVector::ZeroVector;
			StopSliding();
			SetMovementMode(MOVE_Walking);
			StartNewPhysics(timeTick, Iterations);
			break;
		}
		else
		{
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
			{
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
			}
		}
	}
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
	while (PhysValidator(remainingTime, Iterations))
	{
		++Iterations;
		bJustTeleported = false;

		// Time steps taken from delta time
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		WallRunDuration += timeTick;

		// Retain previous location value
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FVector OldVelocity = Velocity;

		// Make sure we are next to a wall. Provide a vertical tolerance for the linetrace since it's possible the server has
		// moved our character slightly since we've begun the wall run. In the event we're right at the top/bottom of a wall. we need this
		// tolerance value so we don't Immediately fall off the wall
		if (!IsNextToWall(LineTraceVerticalTolerance))
		{
			if (WallRunSide == ENavType::Front)
			{
				RunUpLedge(timeTick, Iterations);
			}

			continue;
		}

		// Make sure the required wall run keys are still down
		if (!bForwardKeysAreDown || bWantsToCrouch)
		{
			StopWallRunning();
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
			// GetWallRunDirection();
			const FQuat Quat = UpdatedComponent->GetComponentQuat();
			SafeMoveUpdatedComponent(Delta, Quat, true, Hit);

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
					StopWallRunning();
					WallRunCheck(FVector::ZeroVector, Hit);
					ClimbCheck(FVector::ZeroVector, Hit);
					if (MovementMode == MOVE_None) StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					StartNewPhysics(remainingTime, Iterations);
					return;
				}
			}
			else
			{
				FindWall(UpdatedComponent->GetComponentLocation(), CurrentWall, NULL);
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			Velocity = FVector::ZeroVector;
			StopWallRunning();
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
	while (PhysValidator(remainingTime, Iterations))
	{
		++Iterations;
		bJustTeleported = false;
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
			continue;
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

				StopClimbing();
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

void UGACharacterMovementComponent::PhysVaulting(float DeltaTime, int32 Iterations)
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
	while (PhysValidator(remainingTime, Iterations))
	{
		++Iterations;
		bJustTeleported = false;

		// Time steps taken from delta time
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		WallRunDuration += timeTick;

		// Retain previous location value
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		// Zero out player input
		Acceleration = (VaultEndLocation - OldLocation).GetSafeNormal();
		// Apply acceleration
		CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration());
		// Save new Velocity
		const FVector MoveVelocity = Velocity;
		// Set the owning player's new velocity based on the wall run direction
		// FVector newVelocity = FVector() * MoveVelocity.Size();

		const FVector Delta = MoveVelocity * DeltaTime;
		const bool bZeroDelta = Delta.IsNearlyZero();

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			FHitResult Hit(1.f);
			const FQuat Quat = UpdatedComponent->GetComponentQuat();
			SafeMoveUpdatedComponent(Delta, Quat, true, Hit);

			if (Hit.Time < 1)
			{
				HandleImpact(Hit, DeltaTime, Delta);
				SafeMoveUpdatedComponent(Delta.ProjectOnToNormal(FVector::UpVector), Quat, true, Hit);
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			Velocity = FVector::ZeroVector;
			StopWallRunning();
			StartNewPhysics(timeTick, Iterations);
			return;
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
	const FVector end = start + (2.1 * FVector::DownVector * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	if (LineTrace(traceHit, start, end))
	{
		bClimbingLedge = false;
		StopClimbing();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	FHitResult ForwardHit(1.0f);
	const FRotator Rotation = UpdatedComponent->GetComponentRotation();// ComputeOrientRotationToWall(DeltaTime, CurrentWall.HitResult.ImpactNormal);
	SafeMoveUpdatedComponent(Forward * MaxClimbSpeed * DeltaTime, Rotation, true, ForwardHit);

	if (ForwardHit.bBlockingHit)
	{
		FHitResult UpHit(1.0f);
		SafeMoveUpdatedComponent(FVector::UpVector * MaxClimbSpeed * DeltaTime, Rotation, true, UpHit);

		if (UpHit.bBlockingHit)
		{
			HangCheck();
			return; // Something is preventing us from climbing upward.
		}
	}
}

void UGACharacterMovementComponent::RunUpLedge(float DeltaTime, int32 Iterations)
{
	FHitResult traceHit;
	const FVector Forward = PawnOwner->GetActorForwardVector();
	const FVector start = UpdatedComponent->GetComponentLocation() + (-1 * Forward * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius());
	const FVector end = start + (2.1 * FVector::DownVector * CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	if (LineTrace(traceHit, start, end))
	{
		StopWallRunning();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	FHitResult ForwardHit(1.0f);
	const FRotator Rotation = UpdatedComponent->GetComponentRotation();// ComputeOrientRotationToWall(DeltaTime, CurrentWall.HitResult.ImpactNormal);
	SafeMoveUpdatedComponent(Forward * MaxClimbSpeed * DeltaTime, Rotation, true, ForwardHit);

	if (ForwardHit.bBlockingHit)
	{
		FHitResult UpHit(1.0f);
		SafeMoveUpdatedComponent(FVector::UpVector * MaxClimbSpeed * DeltaTime, Rotation, true, UpHit);

		if (UpHit.bBlockingHit)
		{
			HangCheck();
			return; // Something is preventing us from climbing upward.
		}
	}
}

void UGACharacterMovementComponent::VaultOverLedge()
{

}
#pragma endregion

#pragma region Utility
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

bool UGACharacterMovementComponent::AreRequiredKeysDown() const
{
	// Since this function is checking for input, it should only be called for locally controlled character
	if (!GetPawnOwner()->IsLocallyControlled())
		return false;

	// Make sure the spring key is down (the player may only wall run if he is holding toward wall and forward)
	TArray<FInputAxisKeyMapping> MoveForwardKeyMappings;
	// UInputSettings::GetInputSettings()->GetActionMappingByName();
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

bool UGACharacterMovementComponent::IsStepUpTooHigh(const FHitResult& Hit) const
{
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	// Since we float a variable amount off the floor, we need to enforce max step height off the actual point of impact with the floor.
	const float FloorDist = FMath::Max(0.f, CurrentFloor.GetDistanceToFloor());
	float StepTravelUpHeight = FMath::Max(MaxStepHeight - FloorDist, 0.f);
	float PawnInitialFloorBaseZ = OldLocation.Z - PawnHalfHeight;

	// Don't step up if the impact is below us, accounting for distance from floor.
	if (Hit.ImpactPoint.Z <= PawnInitialFloorBaseZ)
	{
		return false;
	}

	FHitResult StepHit;
	const FVector Start = OldLocation + FVector(0,0, MaxStepHeight);
	const FVector End = Start + Velocity;
	return LineTrace(StepHit, Start, End);
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
	// surface_normal.FindBestAxisVectors(CrossVector, direction); // Maybe we can find a way to replace all these lines with this utiility function?
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
#pragma endregion

#pragma region IsMovementMode
bool UGACharacterMovementComponent::IsCustomMovementMode(uint8 custom_movement_mode) const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == custom_movement_mode;
}

bool UGACharacterMovementComponent::IsMovingOnGround() const
{
	return ((MovementMode == MOVE_Custom && 
		CustomMovementMode == ECustomMovementMode::MOVE_Sliding) || 
		(MovementMode == MOVE_Walking) || 
		(MovementMode == MOVE_NavWalking)) && 
		UpdatedComponent;
}

bool UGACharacterMovementComponent::IsSprinting() const
{
	AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner);
	return GACharacter && GACharacter->bIsSprinting;
}

bool UGACharacterMovementComponent::IsSliding() const
{
	return IsCustomMovementMode(ECustomMovementMode::MOVE_Sliding) && UpdatedComponent;
}

bool UGACharacterMovementComponent::IsMovingOnWall() const
{
	return (MovementMode == EMovementMode::MOVE_Custom) && ((CustomMovementMode == MOVE_Climbing) || (CustomMovementMode == MOVE_WallRunning) || (CustomMovementMode == MOVE_Vaulting)) && UpdatedComponent;
}

bool UGACharacterMovementComponent::IsWallRunning() const
{
	return IsCustomMovementMode(ECustomMovementMode::MOVE_WallRunning) && UpdatedComponent;
}

bool UGACharacterMovementComponent::IsClimbing() const
{
	return IsCustomMovementMode(ECustomMovementMode::MOVE_Climbing) && UpdatedComponent;
}

bool UGACharacterMovementComponent::IsHanging() const
{
	return IsCustomMovementMode(ECustomMovementMode::MOVE_Hanging) && UpdatedComponent;
}
#pragma endregion

#pragma region Getters
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
		case MOVE_Sliding:
			return MaxSlideSpeed;
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
		case MOVE_Sliding:
			return SlideAcceleration;
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
		return BrakingDecelerationWalking;
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
		case MOVE_Sliding:
			return BrakingDecelerationSliding;
		default:
			return 0.f;
		}
	case MOVE_None:
	default:
		return 0.f;
	}
}

float UGACharacterMovementComponent::GetSubmergenceRatio() const
{
	const FVector CurLocation = UpdatedComponent->GetComponentLocation();																		// Our Current Location
	const FVector GravLocation = CurLocation * -GetPawnOwner()->GetGravityDirection();															// Location.Height relative to gravity direction
	float SubmergenceRatio = (WaterLevelZ - GravLocation.Size()) / CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();			// Ratio of Water Height relative to our height

	return SubmergenceRatio;
}
#pragma endregion

#pragma region MovementRequests
void UGACharacterMovementComponent::EnterWater(float InWaterLevelZ)
{
	WaterLevelZ = InWaterLevelZ;
	bIsWading = true;

	UE_LOG(LogTemp, Warning, TEXT("Entered Water"));
}

void UGACharacterMovementComponent::ExitWater()
{
	WaterLevelZ = 0;
	bIsWading = false;
}

bool UGACharacterMovementComponent::CanSlide() const
{
	AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner);
	return CanEverSlide() && IsMovingOnGround() && GACharacter && GACharacter->CanSlide() && GACharacter->bIsSprinting;
}

void UGACharacterMovementComponent::StartSliding()
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
	{
		if (!GACharacter->CanSlide()) return;
		GACharacter->OnBeginSlide();
	}

	SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_Sliding);

	bWantsToSlide = false;
	SlideConsecutiveResetDuration = 0.0f;
	bCanWalkOffLedgesWhenCrouching = true;

	++CurrentSlides;

	const FVector Force =
		(Velocity.GetSafeNormal2D() *
			(SlideInitialForce *
			(bConsecutiveSlidesLoseMomentum ? (1.0f / CurrentSlides) : 1)));
	AddImpulse(Force);
}

void UGACharacterMovementComponent::StopSliding()
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
	{
		GACharacter->OnEndSlide();
	}

	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);

	bWantsToSlide = false;
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

void UGACharacterMovementComponent::StartVaulting()
{
	UE_LOG(LogTemp, Warning, TEXT("Vaulting!"));
	SetMovementMode(MOVE_Custom, MOVE_Vaulting);
}

void UGACharacterMovementComponent::StopVaulting()
{
	SetMovementMode(MOVE_Walking);
}

void UGACharacterMovementComponent::StartWallRunning()
{
	if (!bForwardKeysAreDown || Velocity.Size() <= MaxWalkSpeed)
		return;

	// Convenience if statement for locally checked GACharacter cast
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
	{
		if (GACharacter->CanWallRun()) GACharacter->OnBeginWallRun();
		else return;
	}

	SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_WallRunning);
	WallRunDuration = 0.0f;
}

void UGACharacterMovementComponent::StopWallRunning()
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner)) GACharacter->OnEndWallRun();

	// Set the movement mode back to falling
	SetMovementMode(EMovementMode::MOVE_Falling);
	WallRunDuration = 0.0f;
}

void UGACharacterMovementComponent::StartClimbing()
{	
	if (!bForwardKeysAreDown)
		return;

	// Convenience if statement for locally checked GACharacter cast
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
		if (!GACharacter->CanClimb())
			return;

	SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_Climbing);
}

void UGACharacterMovementComponent::BeginClimbLedge()
{
	bClimbingLedge = true;
}

void UGACharacterMovementComponent::StopClimbing()
{
	SetMovementMode(EMovementMode::MOVE_Falling);
}
#pragma endregion

#pragma region CollisionChecks
bool UGACharacterMovementComponent::LineTrace(FHitResult& Hit, const FVector& start, const FVector& end) const
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner()); // Ignore Self

	bool bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, start, end, ECollisionChannel::ECC_Visibility, Params); // Line Trace

	/** Debugging */
	FColor Color = bBlockingHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), start, end, Color);
	if (bBlockingHit) DrawDebugPoint(GetWorld(), Hit.Location, 10.0f, Color);
	// UE_LOG(LogTemp, Warning, TEXT("%s"), Hit.HitObjectHandle.FetchActor() == nullptr ? *FName().ToString() : *Hit.HitObjectHandle.FetchActor()->GetName());


	// Return Value
	return bBlockingHit;
}

bool UGACharacterMovementComponent::SweepTrace(FHitResult& Hit, const FVector& start, const FVector& end) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(GetOwner()); // Ignore Self
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

	// Wall Section
	if (!CanStepUp(Hit) || IsStepUpTooHigh(Hit))
	{
		VaultCheck(NormalImpulse, Hit);		// Short Walls
		WallRunCheck(NormalImpulse, Hit);	// Run check first
		ClimbCheck(NormalImpulse, Hit);		// Regular climb as last check
	}
}

void UGACharacterMovementComponent::VaultCheck(const FVector& NormalImpulse, const FHitResult& Hit)
{
	if (!IsMovingOnGround() || IsSliding() || !IsSprinting() || 
		!CanSurfaceBeWallRan(Hit.ImpactNormal))
		return;

	FVector FloorBottom = FVector::ZeroVector;
	if (IsNextToWall()) // Wall is at our waist height
	{
		FHitResult TopHit;
		const FVector TopStart = GetActorHeadLocation();
		const FVector TopEnd = TopStart + UpdatedComponent->GetForwardVector() * 70;
		if (LineTrace(TopHit, TopStart, TopEnd)) // Wall is higher than our character
		{
			return;
		}

		FHitResult NewHit;
		const FVector NewEnd = GetActorLocation() + UpdatedComponent->GetForwardVector() * 70;
		LineTrace(NewHit, TopHit.Location, NewEnd);

		FloorBottom = NewHit.Location;
	}
	else // Wall is below our waist
	{
		FHitResult BottomHit;
		const FVector BottomStart = GetActorFeetLocation();
		const FVector BottomEnd = BottomStart + UpdatedComponent->GetForwardVector() * 70;
		if (!LineTrace(BottomHit, BottomStart, BottomEnd)) // There is no obstructing wall
		{
			return;
		}

		FloorBottom = BottomHit.Location;
	}

	FHitResult TopHit;
	const FVector TopEnd = FloorBottom + FVector(0, 0, GetActorHeadLocation().Z);
	if (LineTrace(TopHit, FloorBottom, TopEnd)) // Check our character can fit on the ledge
	{
		if ((FloorBottom - TopHit.Location).Size() < GetCrouchedHalfHeight()) // Our character cannot fit even if crouched
		{
			return;
		}
		else // We can fit, but only crouched
		{
			Crouch(false);
		}
	}

	VaultEndLocation = FloorBottom;
	StartVaulting();
}

void UGACharacterMovementComponent::WallRunCheck(const FVector& NormalImpulse, const FHitResult& Hit)
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
		if (!GACharacter->CanWallRun())
			return;

	if (!CanEverWallRun() ||
		IsMovingOnWall() || IsSwimming() ||
		!CanSurfaceBeWallRan(Hit.ImpactNormal)) return;

	FindWallRunDirectionAndSide(Hit.ImpactNormal, WallRunDirection, WallRunSide);
	if (IsFalling() && Velocity.Z <= 1.f)
	{
		if (WallRunSide == ENavType::Front) return;
	}
	else
	{
		if (WallRunSide != ENavType::Front) return;
	}
	if (!IsNextToWall()) return;

	StartWallRunning();
}

void UGACharacterMovementComponent::ClimbCheck(const FVector& NormalImpulse, const FHitResult& Hit)
{
	if (AGACharacter* GACharacter = Cast<AGACharacter>(CharacterOwner))
		if (!GACharacter->CanClimb())
			return;

	if (!CanEverClimb() || 
		IsMovingOnWall() || !IsFalling() || IsCrouching() || IsSwimming() ||
		!IsInFrontOfWall()) return;

	WallRunDirection = FVector::UpVector;
	WallRunSide = ENavType::Front;

	StartClimbing();
}

void UGACharacterMovementComponent::HangCheck()
{
	if (IsMovingOnGround()) return;
}

bool UGACharacterMovementComponent::IsInFrontOfWall(float vertical_tolerance)
{
	float CapsuleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector traceStart = GetPawnOwner()->GetActorLocation(); // +(WallRunDirection * 20.0f);
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
					StopWallRunning();
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
				StopWallRunning();
				SetMovementMode(MOVE_Falling);
				return true;
			}

			if (IsClimbing())
			{
				Velocity += ((FVector::ZeroVector - PawnOwner->GetActorForwardVector()) + FVector::UpVector) * JumpZVelocity;
				StopClimbing();
				SetMovementMode(MOVE_Falling);
				return true;
			}

			if (IsSliding())
			{
				StopSliding();

				if (Velocity.Size() > MaxWalkSpeed)
				{
					Velocity += (PawnOwner->GetActorForwardVector() + FVector::UpVector * 0.25).GetSafeNormal() * JumpZVelocity * 2;
					SetMovementMode(MOVE_Falling);
					return true;
				}
				else
				{
					return Super::DoJump(bReplayingMoves);
				}
			}

			if (IsCrouching())
			{
				Velocity += FVector::UpVector * JumpZVelocity * 2;
				CharacterOwner->UnCrouch(bReplayingMoves);
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
	if (IsWallRunning()) StopWallRunning();

	// if we landed while climbing, make sure we stop climbing
	else if (IsClimbing()) StopClimbing();

	// If we land and want to slide, then we slide. Otherwise return to walking
	if (bWantsToSlide)
	{
		StartSliding();
	}
	else
	{
		SetMovementMode(MOVE_Walking);
	}

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
		if (CanSlide() && !IsSliding())
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
