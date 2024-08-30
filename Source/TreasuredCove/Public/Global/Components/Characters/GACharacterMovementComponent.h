// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavType.h"
#include "GACharacterMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FFindWallResult
{
	GENERATED_BODY()

public:
	/**
	* True if there was a blocking hit in the floor test that was NOT in initial penetration.
	* The HitResult can give more info about other circumstances.
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
		uint32 bBlockingHit : 1;

	/** True if the hit found a valid climbable wall. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
		uint32 bClimbableWall : 1;

	/** True if the hit found a valid climbable wall using a line trace (rather than a sweep test, which happens when the sweep test fails to yield a climbable surface). */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
		uint32 bLineTrace : 1;

	/** The distance to the wall, computed from the swept capsule trace. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
		float WallDist;

	/** The distance to the wall, computed from the trace. Only valid if bLineTrace is true. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
		float LineDist;

	/** Hit result of the test that found a floor. Includes more specific data about the point of impact and surface normal at that point. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = CharacterFloor)
		FHitResult HitResult;

public:

	FFindWallResult()
		: bBlockingHit(false)
		, bClimbableWall(false)
		, bLineTrace(false)
		, WallDist(0.f)
		, LineDist(0.f)
		, HitResult(1.f)
	{
	}

	/** Returns true if the floor result hit a walkable surface. */
	bool IsClimbableWall() const
	{
		return bBlockingHit && bClimbableWall;
	}

	void Clear()
	{
		bBlockingHit = false;
		bClimbableWall = false;
		bLineTrace = false;
		WallDist = 0.f;
		LineDist = 0.f;
		HitResult.Reset(1.f, false);
	}

	/** Gets the distance to floor, either LineDist or FloorDist. */
	float GetDistanceToWall() const
	{
		// When the floor distance is set using SetFromSweep, the LineDist value will be reset.
		// However, when SetLineFromTrace is used, there's no guarantee that FloorDist is set.
		return bLineTrace ? LineDist : WallDist;
	}

	void SetFromSweep(const FHitResult& InHit, const float InSweepWallDist, const bool bIsClimbableWall);
	void SetFromLineTrace(const FHitResult& InHit, const float InLineDist, const bool bIsClimbableWall);
};

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FWallStepDownResult
{
	GENERATED_BODY()

public:
	uint32 bComputedWall : 1;
	FFindWallResult WallResult;

	FWallStepDownResult()
		: bComputedWall(false)
	{
	}
};

enum class EWalkingGait : uint8
{
	GAIT_Walk,
	GAIT_Sprint,
	GAIT_Crouch,
	GAIT_Prone,
};

enum class ESwimmingGait : uint8
{
	GAIT_SurfaceSwim,
	GAIT_UnderwaterSwim,
	GAIT_WallSwim,
};


/** This class is being designed in tandem with ReidsChannel (YouTube)
 * The WallRunning logic comes from his tutorial but it has been moved from the ACharacter to here.
 */
UCLASS()
class TREASUREDCOVE_API UGACharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_GA;
	
#pragma region Defaults
private:
	// Wading Members
	uint8 bIsWading:1;
	float WaterLevelZ = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking|Prone", Meta = (AllowPrivateAccess = "true"))
	float MaxWadingSpeedReduction = 0.5;

	// Prone Members
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking|Prone", Meta = (AllowPrivateAccess = "true"))
	uint8 bCanProne:1;
	uint8 bIsProne:1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking|Prone", Meta = (AllowPrivateAccess = "true"))
	float MaxProneSpeed = 100.f;

	// Sprinting members
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking|Sprinting", Meta = (AllowPrivateAccess = "true"))
	uint8 bCanSprint:1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking|Sprinting", Meta = (AllowPrivateAccess = "true"))
	float MaxSprintSpeed = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking|Sprinting", Meta = (AllowPrivateAccess = "true"))
	float SprintAcceleration = 2000.0f;

	// Sliding members
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	uint8 bCanSlide:1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	uint8 bConsecutiveSlidesLoseMomentum:1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	float MaxSlideSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	float SlideInitialForce = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	float SlideAcceleration = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	float SlideGroundFriction = 1.3f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	float BrakingDecelerationSliding = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	float SlideConsecutiveResetDelay = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	float SlideGravityMultiplier = 2.0f;

	// Rolling Members
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Sliding", Meta = (AllowPrivateAccess = "true"))
	uint8 bCanRoll:1;
	uint8 bIsRolling:1;

	// Wall running members
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	uint8 bCanWallRun:1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float LineTraceVerticalTolerance = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float MaxWallRunSpeed = 2000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float ForwardWallRunThreshold = 0.3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float MaxWallRunDuration = 0.5;

	// Climbing members
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Climbing", Meta = (AllowPrivateAccess = "true"))
	bool bCanClimb = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Climbing", Meta = (AllowPrivateAccess = "true"))
	float MaxClimbSpeed = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Climbing", Meta = (AllowPrivateAccess = "true"))
	float BrakingDecelerationClimbing = 4096;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Climbing", Meta = (AllowPrivateAccess = "true"))
	float LedgeMaxHeight = 45.f;

	// Vaulting Members
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Custom Movement|Vaulting", Meta = (AllowPrivateAccess = "true"))
	uint8 bCanVault:1;
#pragma endregion

#pragma region Overrides
public:
	//Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//End UActorComponent Interface

	//BEGIN UCharacterMovementComponent Interface

	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual void UpdateFromCompressedFlags(uint8 flags) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual bool IsMovingOnGround() const override;
	
	// BEGIN Jumping
	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	// END Jumping
	
	// BEGIN Crouching
	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;
	// END Crouching

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	//END UCharacterMovementComponent Interface
#pragma endregion

#pragma region NewFunctions
public:
	/** Returns location of controlled actor's "feet" meaning center of bottom of collision bounding box */
	virtual FVector GetActorHeadLocation() const { return UpdatedComponent ? (UpdatedComponent->GetComponentLocation() + FVector(0, 0, UpdatedComponent->Bounds.BoxExtent.Z)) : FNavigationSystem::InvalidLocation; }
	// Returns true if the movement mode is custom and matches the provided custom movement mode
	bool IsCustomMovementMode(uint8 custom_movement_mode) const;
	// Helper function performs GetWorld()->SingleLineTrace() by visibility 
	bool LineTrace(FHitResult& Hit, const FVector& start, const FVector& end) const;
	// Helper function performs GetWorld()->SweepSingleByChannel() by visibility using owner's primitive cylinder
	bool SweepTrace(FHitResult& Hit, const FVector& start, const FVector& end) const;
	// Sets the gait for the walking movement mode. (walk, sprint, crouch, etc)
	void SetMovementGait_Walking(uint8 NewGait);
	// Returns evaluation of common booleans required for performing movement physics
	bool PhysValidator(float remainingTime, int32 Iterations);
#pragma endregion

#pragma region SwimmingRelated

#pragma region Wading
public:
	// 
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void EnterWater(float InWaterLevelZ);
	//
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void ExitWater();
	//
	float GetSubmergenceRatio() const;

#pragma endregion
#pragma endregion

#pragma region Sliding
public:
	//
	FORCEINLINE bool CanEverSlide() const { return bCanSlide; };
	//
	bool CanSlide() const;
	// Called when trying to crouch while moving faster than crouch speed or after jumping.
	// Adds an Impulse
	void StartSliding();
	// Called after a jump during a slide or when slowed down to walking speed.
	void StopSliding();
	// Sets CurrentSlides to 0
	void ResetSliding();
	// Returns true if we are in a custom movement mode, that movement mode is Sliding, and UpdatedComponent is valid
	bool IsSliding() const;
	// Find acceleration based off of current slope
	FVector CalculateFloorInfluence(const FVector& InSlope) const;
	// Velocity Calculator for sliding
	void PhysSliding(float DeltaTime, int32 Iterations);
#pragma endregion

#pragma region Sprinting
public:
	// Allows the player to sprint if true, and does not if false.
	FORCEINLINE UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void SetCanSprint(bool bNewCanSprint) { bCanSprint = bNewCanSprint; };
	//
	FORCEINLINE bool CanEverSprint() const { return bCanSprint; };
	//
	bool IsSprinting() const;
	// Sets sprinting to be enabled or disabled
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void StopSprinting();
#pragma endregion

#pragma region WallRelated
public:
	//
	FRotator ComputeOrientRotationToWall(float DeltaTime, const FVector& SurfaceNormal) const;
	//
	bool IsMovingOnWall() const;
	//
	void ComputeWallDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindWallResult& OutWallResult, float SweepRadius, const FHitResult* ForwardSweepResult = NULL) const;
	//
	void FindWall(const FVector& CapsuleLocation, FFindWallResult& OutWallResult, bool bCanUseCachedLocation, const FHitResult* ForwardSweepResult = NULL) const;
	//
	bool IsStepUpTooHigh(const FHitResult& Hit) const;

#pragma region Ledges
public:
	// 
	FORCEINLINE bool CanEverVault() const { return bCanVault; };
	// Checks for short walls we run may into
	void CatchLedgeCheck();
	// Checks for short walls we can vault
	void VaultCheck(const FVector& NormalImpulse, const FHitResult& Hit);
	// For Running onto a ledge after vertically wall-running up a wall
	void RunUpLedge(float DeltaTime, int32 Iterations);
	// For moving onto a ledge while climbing
	// void ClimbUpLedge();
	// For crossing a short wall while walking/running
	void VaultOverLedge(float DeltaTime, int32 Iterations);
	//
	void StartVaulting();
	//
	void StopVaulting();
	// 
	void PhysVaulting(float DeltaTime, int32 Iterations);
#pragma endregion

#pragma region WallRunning
public:
	//
	ENavType GetWallRunSide() const { return WallRunSide; };
	//
	FVector GetWallRunDirection() const { return WallRunDirection; };
	//
	FORCEINLINE bool CanEverWallRun() const { return bCanWallRun; };
	// Requests that the player begins wall running
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void StartWallRunning();
	// Ends the characters wall run
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void StopWallRunning();
	// Reutrns true if the keys required for wall running are down
	bool AreRequiredKeysDown() const;
	// Returns true if the player is next to a wall that can be wall run
	bool IsNextToWall(float vertical_tolerance = 0.0f, float horizontal_tolerance = 0.0f);
	// Finds the wall run direction and side based on the specified surface normal
	void FindWallRunDirectionAndSide(const FVector& surface_normal, FVector& direction, ENavType& side) const;
	// Helper function that returns true if the specified surface normal can be wall run on
	bool CanSurfaceBeWallRan(const FVector& surface_normal) const;
	// Actual Velocity setting function for Wall Running
	void PhysWallRunning(float DeltaTime, int32 Iterations);
	// Returns true if we are in a custom movement mode, that movement mode is Wall Running, and UpdatedComponent is valid
	bool IsWallRunning() const;
	// Checks if we are meet specific conditions to start wall running
	void WallRunCheck(const FVector& NormalImpulse, const FHitResult& Hit);
private:
	// Called when the owning actor hits something
	UFUNCTION()
	void OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
#pragma endregion

#pragma region Climbing
public:
	//
	FORCEINLINE bool CanEverClimb() const { return bCanClimb; };
	// Requests the player to start climbing
	void StartClimbing();
	// Called to set Movement mode off of climbing
	void StopClimbing();
	// Called when climbing vertically and a ledge is detected
	void BeginClimbLedge();
	// Returns true if the player is in front of a wall
	bool IsInFrontOfWall(float vertical_tolerance = 0.0f);
	// Actual velocity calulcator for climbing
	void PhysClimbing(float DeltaTime, int32 Iterations);
	// 
	void MoveAlongWall(const FVector& InVelocity, float DeltaSeconds, FHitResult* OutHit);
	//
	void ClimbLedge(float DeltaTime, int32 Iterations);
	// 
	FVector ComputeWallMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const;
	// Returns true if we are in a custom movement mode, that movement mode is Climbing, and UpdatedComponent is valid
	bool IsClimbing() const;
	// Simply the inverse result of IsWalkable()
	bool IsClimbable(const FHitResult& Hit) const;
	// Checks if we are meet specific conditions to start climbing
	void ClimbCheck(const FVector& NormalImpulse, const FHitResult& Hit);
#pragma endregion

#pragma region Hanging
	//
	void HangCheck();
	//
	bool IsHanging() const;
#pragma endregion
#pragma endregion

#pragma region CompressedFlags
private:
	uint8 bWantsToSprint:1;
	uint8 bWantsToSlide:1;
	uint8 bForwardKeysAreDown:1;
#pragma endregion

#pragma region PrivateVariables
private:
	// The value at beginplay
	bool bDefaultUseControllerRotationYaw = false;
	// The value at beginplay
	bool bDefaultOrientRotationToMovement = true;
	// Attached to ledge
	bool bClimbingLedge = false;
	// Holds the value before we enter Wall Climbing state
	bool bPrevOrientRotationToMovement = false;
	// Holds the value before we enter Wall Climbing state
	bool bPrevUseControllerRotationYaw = false;
	// True if the sprint key is down
	bool bSprintKeyDown = false;

	// Amount of consecutive slides
	uint32 CurrentSlides = 0;

	// The value at beginplay
	float DefaultGroundFriction = 0.f;
	// The amount of time spent on current wall and traveling in current direction.
	float WallRunDuration = 0.0f;
	// The amount of time before resetting slide strength
	float SlideConsecutiveResetDuration = 0.0f;

	// The value at begin play
	FVector DefaultMeshRelativeLocation = FVector::ZeroVector;
	// Location set at the start of a vault that the character will move toward.
	FVector VaultEndLocation = FVector::ZeroVector;
	// The direction the character is currently wall running in.
	FVector WallRunDirection;
	// The surface normal of the wall before climbing it as a ledge.
	FVector WallLedgeNormal;
	// The side of the wall the player is running on.
	ENavType WallRunSide;
	// Comparable to CurrentFloor but instead for walls
	FFindWallResult CurrentWall;
#pragma endregion
};

class FSavedMove_GA : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	// Resets all variables
	virtual void Clear() override;
	// Store input commands in the compressed flags
	virtual uint8 GetCompressedFlags() const override;
	// this is used to check whether or not two moves can be combined into one.
	// Basically you just check to make sure that the saved variables are the same.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* Character, float MaxDelta) const override;
	// Sets up the move before secding it to the server.
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	// Sets variables on character movement component
	virtual void PrepMoveFor(class ACharacter* Character) override;

private:
	uint8 SavedWantsToSprint : 1;
	uint8 SavedWallRunKeysDown : 1;
};

class FNetworkPredictionData_Client_GA : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;
	FNetworkPredictionData_Client_GA(const UCharacterMovementComponent& ClientMovement);
	virtual FSavedMovePtr AllocateNewMove() override;
};