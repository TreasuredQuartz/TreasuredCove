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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float MaxSprintSpeed = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Grounded", Meta = (AllowPrivateAccess = "true"))
	float SprintAcceleration = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float LineTraceVerticalTolerance = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float MaxWallRunSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float ForwardWallRunThreshold = 0.25;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float MaxWallRunDuration = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Movement|Wall Run", Meta = (AllowPrivateAccess = "true"))
	float MaxClimbSpeed = 200.f;
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
	virtual void UpdateFromCompressedFlags(uint8 flags) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves) override;
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	//END UCharacterMovementComponent Interface
#pragma endregion

#pragma region NewFunctions
public:
	// Returns true if the movement mode is custom and matches the provided custom movement mode
	bool IsCustomMovementMode(uint8 custom_movement_mode) const;
	// Helper function performs GetWorld()->SingleLineTrace() by visibility 
	bool LineTrace(FHitResult& Hit, const FVector& start, const FVector& end) const;
	// Helper function performs GetWorld()->SweepSingleByChannel() by visibility using owner's primitive cylinder
	bool SweepTrace(FHitResult& Hit, const FVector& start, const FVector& end) const;
#pragma endregion

#pragma region Sprinting
public:
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
#pragma region WallRunning
public:
	// Requests that the player begins wall running
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	bool BeginWallRun();
	// Ends the characters wall run
	UFUNCTION(BlueprintCallable, Category = "Character|Movement")
	void EndWallRun();
	// Reutrns true if the keys required for wall running are down
	bool AreRequiredKeysDown() const;
	//Returns true if the player is next to a wall that can be wall run
	bool IsNextToWall(float vertical_tolerance = 0.0f);
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
	// Requests the player to start climbing
	bool BeginClimb();
	// Called to set Movement mode off of climbing
	void EndClimb();
	// Called when climbing vertically and a ledge is detected
	void BeginClimbLedge();
	// Returns true if the player is in front of a wall
	bool IsInFrontOfWall(float vertical_tolerance = 0.0f);
	// Actual velocity calulcator for climbing
	void PhysClimbing(float DeltaTime, int32 Iterations);
	// 
	void MoveAlongWall(float DeltaTime, int32 Iterations);
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
#pragma endregion

#pragma region CompressedFlags
private:
	uint8 bWantsToSprint : 1;
	uint8 bForwardKeysAreDown : 1;
#pragma endregion

#pragma region PrivateVariables
private:
	//
	bool bClimbingLedge = false;
	// Holds the value before we enter Wall Climbing state
	bool bPrevOrientRotationToMovement;
	// True if the sprint key is down
	bool bSprintKeyDown = false;
	// The amount of time spent on current wall and traveling in current direction.
	float WallRunDuration = 0.0f;
	// The direction the character is currently wall running in.
	FVector WallRunDirection;
	// The side of the wall the player is running on.
	ENavType WallRunSide;
	//
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