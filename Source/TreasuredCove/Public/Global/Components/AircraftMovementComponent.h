// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleMovementComponent.h"
#include "Global/Components/Vehicles/VehicleMovementReplication.h"
#include "AircraftMovementComponent.generated.h"

/** Shared pointer for easy memory management of FSavedMove_Character, for accumulating and replaying network moves. */
// typedef TSharedPtr<class FSavedMove_Vehicle> FSavedMovePtr;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UAircraftMovementComponent : public UVehicleMovementComponent
{
	GENERATED_BODY()

public:
	UAircraftMovementComponent();

private:
	class AGAVehicle* VehicleOwner;

	uint8 bAutoPilot:1;
	uint8 bOrientRotationToMovement:1;
	uint8 bUseControllerDesiredRotation:1;
	uint8 bForceNextFloorCheck:1;
	uint8 bMovementInProgress:1;

	float Speed;
	float Thrust;
	float ThrustScale;
	float MinThrustToOvercomeGravity;
	float MaxThrust;

	FVector Acceleration;
	FVector PendingForceToApply;
	FVector PendingImpulseToApply;
	FVector PendingLaunchVelocity;

	FRotator RotationRate;

public:
	/**
	 * Actor's current movement mode (walking, falling, etc).
	 *    - walking:  Walking on a surface, under the effects of friction, and able to "step up" barriers. Vertical velocity is zero.
	 *    - falling:  Falling under the effects of gravity, after jumping or walking off the edge of a surface.
	 *    - flying:   Flying, ignoring the effects of gravity.
	 *    - swimming: Swimming through a fluid volume, under the effects of gravity and buoyancy.
	 *    - custom:   User-defined custom movement mode, including many possible sub-modes.
	 * This is automatically replicated through the Character owner and for client-server movement functions.
	 * @see SetMovementMode(), CustomMovementMode
	 */
	UPROPERTY(Category = "Character Movement: MovementMode", BlueprintReadOnly)
	TEnumAsByte<enum EMovementMode> MovementMode;

public:
	/**
	 * Compute remaining time step given remaining time and current iterations.
	 * The last iteration (limited by MaxSimulationIterations) always returns the remaining time, which may violate MaxSimulationTimeStep.
	 *
	 * @param RemainingTime		Remaining time in the tick.
	 * @param Iterations		Current iteration of the tick (starting at 1).
	 * @return The remaining time step to use for the next sub-step of iteration.
	 * @see MaxSimulationTimeStep, MaxSimulationIterations
	 */
	float GetSimulationTimeStep(float RemainingTime, int32 Iterations) const;

	/**
	 * Max time delta for each discrete simulation step.
	 * Used primarily in the the more advanced movement modes that break up larger time steps (usually those applying gravity such as falling and walking).
	 * Lowering this value can address issues with fast-moving objects or complex collision scenarios, at the cost of performance.
	 *
	 * WARNING: if (MaxSimulationTimeStep * MaxSimulationIterations) is too low for the min framerate, the last simulation step may exceed MaxSimulationTimeStep to complete the simulation.
	 * @see MaxSimulationIterations
	 */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "0.0166", ClampMax = "0.50", UIMin = "0.0166", UIMax = "0.50"))
	float MaxSimulationTimeStep;

	/**
	 * Max number of iterations used for each discrete simulation step.
	 * Used primarily in the the more advanced movement modes that break up larger time steps (usually those applying gravity such as falling and walking).
	 * Increasing this value can address issues with fast-moving objects or complex collision scenarios, at the cost of performance.
	 *
	 * WARNING: if (MaxSimulationTimeStep * MaxSimulationIterations) is too low for the min framerate, the last simulation step may exceed MaxSimulationTimeStep to complete the simulation.
	 * @see MaxSimulationTimeStep
	 */
	UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "1", ClampMax = "25", UIMin = "1", UIMax = "25"))
	int32 MaxSimulationIterations;

public:
	virtual void PostLoad() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool HasValidData() const;

private:
	class FNetworkPredictionData_Client_Vehicle* ClientPredictionData;
	class FNetworkPredictionData_Server_Vehicle* ServerPredictionData;

	/**
	 * Set custom struct used for server response RPC serialization.
	 * This is typically set in the constructor for this component and should persist for the lifetime of the component.
	 * @see GetMoveResponseDataContainer()
	 */
	// void SetMoveResponseDataContainer(FVehicleMoveResponseDataContainer& PersistentDataStorage) { MoveResponseDataContainerPtr = &PersistentDataStorage; }

	/**
	 * Get the struct used for server response RPC serialization.
	 * @see SetMoveResponseDataContainer(), ClientHandleMoveResponse(), ServerSendMoveResponse().
	 */
	// FVehicleMoveResponseDataContainer& GetMoveResponseDataContainer() const { return *MoveResponseDataContainerPtr; }

	/**
	 * Used internally to save the SavedMove currently being replayed on the client so it is accessible to any functions that might need it.
	 * @see: ClientUpdatePositionAfterServerUpdate
	 */
	void SetCurrentReplayedSavedMove(FSavedMove_Vehicle* SavedMove) { CurrentReplayedSavedMove = SavedMove; }

public:

	/**
	 * Gets the SavedMove being replayed on the client after a correction is received.
	 * @see: ClientUpdatePositionAfterServerUpdate
	 */
	const FSavedMove_Vehicle* GetCurrentReplayedSavedMove() const { return CurrentReplayedSavedMove; }

private:

	/** Current SavedMove being replayed on the client after a correction is received */
	FSavedMove_Vehicle* CurrentReplayedSavedMove = nullptr;

	//////////////////////////////////////////////////////////////////////////
	// Server move data

	/** Default client to server move RPC data container. Can be bypassed via SetNetworkMoveDataContainer(). */
	FVehicleNetworkMoveDataContainer DefaultNetworkMoveDataContainer;

	/** Pointer to server move RPC data container. */
	FVehicleNetworkMoveDataContainer* NetworkMoveDataContainerPtr;

	/** Used for writing server move RPC bits. */
	FNetBitWriter ServerMoveBitWriter;

	/** Used for reading server move RPC bits. */
	FNetBitReader ServerMoveBitReader;

	/** Current network move data being processed or handled within the NetworkMoveDataContainer. */
	FVehicleNetworkMoveData* CurrentNetworkMoveData;

	//////////////////////////////////////////////////////////////////////////
	// Server response data

	/** Default server response RPC data container. Can be bypassed via SetMoveResponseDataContainer(). */
	FVehicleMoveResponseDataContainer DefaultMoveResponseDataContainer;

	/** Pointer to server response RPC data container. */
	FVehicleMoveResponseDataContainer MoveResponseDataContainerPtr;

	/** Used for writing server response RPC bits. */
	FNetBitWriter MoveResponseBitWriter;

	/** Used for reading server response RPC bits. */
	FNetBitReader MoveResponseBitReader;

public:
	/**
	 * If true, high-level movement updates will be wrapped in a movement scope that accumulates updates and defers a bulk of the work until the end.
	 * When enabled, touch and hit events will not be triggered until the end of multiple moves within an update, which can improve performance.
	 *
	 * @see FScopedMovementUpdate
	 */
	UPROPERTY(Category = "Vehicle Movement (General Settings)", EditAnywhere, AdvancedDisplay)
	uint8 bEnableScopedMovementUpdates:1;

	/**
	 * Optional scoped movement update to combine moves for cheaper performance on the server when the client sends two moves in one packet.
	 * Be warned that since this wraps a larger scope than is normally done with bEnableScopedMovementUpdates, this can result in subtle changes in behavior
	 * in regards to when overlap events are handled, when attached components are moved, etc.
	 *
	 * @see bEnableScopedMovementUpdates
	 */
	UPROPERTY(Category = "Vehicle Movement (General Settings)", EditAnywhere, AdvancedDisplay)
	uint8 bEnableServerDualMoveScopedMovementUpdates:1;

public:

	/** Get prediction data for a client game. Should not be used if not running as a client. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Client_Vehicle. */
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const;
	/** Get prediction data for a server game. Should not be used if not running as a server. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Server_Vehicle. */
	virtual class FNetworkPredictionData_Server* GetPredictionData_Server() const;

	FNetworkPredictionData_Client_Vehicle* GetPredictionData_Client_Vehicle() const;
	FNetworkPredictionData_Server_Vehicle* GetPredictionData_Server_Vehicle() const;

	bool HasPredictionData_Client() const;
	bool HasPredictionData_Server() const;

	void CallServerMovePacked(const FSavedMove_Vehicle* NewMove, const FSavedMove_Vehicle* PendingMove, const FSavedMove_Vehicle* OldMove);
	void CallServerMove(const FSavedMove_Vehicle* NewMove, const FSavedMove_Vehicle* OldMove);

	void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel);
	void SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation, const FVector& NewLocation, const FQuat& NewRotation);

	virtual void ServerMove_PerformMovement(const FVehicleNetworkMoveData& MoveData);
	void ServerMovePacked_ClientSend(const FVehicleServerMovePackedBits& PackedBits);
	void ServerMovePacked_ServerReceive(const FVehicleServerMovePackedBits& PackedBits);
	void ServerMove_HandleMoveData(const FVehicleNetworkMoveDataContainer& MoveDataContainer);

	/** Minimum time between client TimeStamp resets.
	 !! This has to be large enough so that we don't confuse the server if the client can stall or timeout.
	 We do this as we use floats for TimeStamps, and server derives DeltaTime from two TimeStamps.
	 As time goes on, accuracy decreases from those floating point numbers.
	 So we trigger a TimeStamp reset at regular intervals to maintain a high level of accuracy. */
	UPROPERTY()
	float MinTimeBetweenTimeStampResets;

	bool VerifyClientTimeStamp(float TimeStamp, FNetworkPredictionData_Server_Vehicle& ServerData);

protected:
	/** Clock time on the server of the last timestamp reset. */
	float LastTimeStampResetServerTime;

	void ProcessClientTimeStampForTimeDiscrepancy(float ClientTimeStamp, FNetworkPredictionData_Server_Vehicle& ServerData);
	bool IsClientTimeStampValid(float TimeStamp, const FNetworkPredictionData_Server_Vehicle& ServerData, bool& bTimeStampResetDetected) const;
	void OnClientTimeStampResetDetected();
	void OnTimeDiscrepancyDetected(float CurrentTimeDiscrepancy, float LifetimeRawTimeDiscrepancy, float Lifetime, float CurrentMoveError);

protected:
	/**
		* Set custom struct used for client to server move RPC serialization.
		* This is typically set in the constructor for this component and should persist for the lifetime of the component.
		* @see GetNetworkMoveDataContainer(), ServerMovePacked_ServerReceive(), ServerMove_HandleMoveData()
		*/
	void SetNetworkMoveDataContainer(FVehicleNetworkMoveDataContainer& PersistentDataStorage) { NetworkMoveDataContainerPtr = &PersistentDataStorage; }

	/**
		* Get the struct used for client to server move RPC serialization.
		* @see SetNetworkMoveDataContainer()
		*/
	FVehicleNetworkMoveDataContainer& GetNetworkMoveDataContainer() const { return *NetworkMoveDataContainerPtr; }

	/**
	 * Current move data being processed or handled.
	 * This is set before MoveAutonomous (for replayed moves and server moves), and cleared thereafter.
	 * Useful for being able to access custom movement data during internal movement functions such as MoveAutonomous() or UpdateFromCompressedFlags() to be able to maintain backwards API compatibility.
	 */
	FVehicleNetworkMoveData* GetCurrentNetworkMoveData() const { return CurrentNetworkMoveData; }

	/**
	 * Used internally to set the FCharacterNetworkMoveData currently being processed, either being serialized or replayed on the client, or being received and processed on the server.
	 * @see GetCurrentNetworkMoveData()
	 */
	void SetCurrentNetworkMoveData(FVehicleNetworkMoveData* CurrentData) { CurrentNetworkMoveData = CurrentData; }

public:
	void ControlledVehicleMove(const FVector& InInputVector, float DeltaTime);
	void PerformMovement(float DeltaTime);
	void ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration);
	virtual void UpdateVehicleStateBeforeMovement(float DeltaSeconds);
	virtual void UpdateVehicleStateAfterMovement(float DeltaSeconds);
	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity);

	FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const;
	FVector ScaleInputAcceleration(const FVector& InputAcceleration) const;

	FRotator GetDeltaRotation(float DeltaTime) const;
	FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const;

	void ApplyAccumulatedForces(float DeltaTime);
	bool HandlePendingLaunch();
	void ClearAccumulatedForces();
	bool ShouldRemainVertical() const;
	void PhysicsRotation(float DeltaTime);
	void StartNewPhysics(float DeltaTime, int32 Iterations);
	void PhysFlying(float DeltaTime, int32 Iterations);

	float GetMaxAcceleration() const;

	void AddThrust(float Val);

public:
	/** Minimum delta time considered when ticking. Delta times below this are not considered. This is a very small non-zero value to avoid potential divide-by-zero in simulation code. */
	static const float MIN_TICK_TIME;

public:
	FString GetMovementName() const;
};

/** FSavedMove_Character represents a saved move on the client that has been sent to the server and might need to be played back. */
class TREASUREDCOVE_API FSavedMove_Vehicle
{
public:
	FSavedMove_Vehicle();
	virtual ~FSavedMove_Vehicle();

	// UE_DEPRECATED_FORGAME(4.20)
	FSavedMove_Vehicle(const FSavedMove_Vehicle&);
	FSavedMove_Vehicle(FSavedMove_Vehicle&&);
	FSavedMove_Vehicle& operator=(const FSavedMove_Vehicle&);
	FSavedMove_Vehicle& operator=(FSavedMove_Vehicle&&);

	AGAVehicle* VehicleOwner;

	uint32 bPressedJump : 1;
	uint32 bWantsToCrouch : 1;
	uint32 bForceMaxAccel : 1;

	/** If true, can't combine this move with another move. */
	uint32 bForceNoCombine : 1;

	/** If true this move is using an old TimeStamp, before a reset occurred. */
	uint32 bOldTimeStampBeforeReset : 1;

	float TimeStamp;    // Time of this move.
	float DeltaTime;    // amount of time for this move
	float CustomTimeDilation;

	// Information at the start of the move
	uint8 StartPackedMovementMode;
	FVector StartLocation;
	FVector StartRelativeLocation;
	FVector StartVelocity;
	// FFindFloorResult StartFloor;
	FRotator StartRotation;
	FRotator StartControlRotation;
	FQuat StartBaseRotation;	// rotation of the base component (or bone), only saved if it can move.
	float StartCapsuleRadius;
	float StartCapsuleHalfHeight;
	TWeakObjectPtr<UPrimitiveComponent> StartBase;
	FName StartBoneName;
	uint32 StartActorOverlapCounter;
	uint32 StartComponentOverlapCounter;
	TWeakObjectPtr<USceneComponent> StartAttachParent;
	FName StartAttachSocketName;
	FVector StartAttachRelativeLocation;
	FRotator StartAttachRelativeRotation;

	// Information after the move has been performed
	uint8 EndPackedMovementMode;
	FVector SavedLocation;
	FRotator SavedRotation;
	FVector SavedVelocity;
	FVector SavedRelativeLocation;
	FRotator SavedControlRotation;
	TWeakObjectPtr<UPrimitiveComponent> EndBase;
	FName EndBoneName;
	uint32 EndActorOverlapCounter;
	uint32 EndComponentOverlapCounter;
	TWeakObjectPtr<USceneComponent> EndAttachParent;
	FName EndAttachSocketName;
	FVector EndAttachRelativeLocation;
	FRotator EndAttachRelativeRotation;

	FVector Acceleration;
	float MaxSpeed;

	// Cached to speed up iteration over IsImportantMove().
	FVector AccelNormal;
	float AccelMag;

	/** Threshold for deciding this is an "important" move based on DP with last acked acceleration. */
	float AccelDotThreshold;
	/** Threshold for deciding is this is an important move because acceleration magnitude has changed too much */
	float AccelMagThreshold;
	/** Threshold for deciding if we can combine two moves, true if cosine of angle between them is <= this. */
	float AccelDotThresholdCombine;
	/** Client saved moves will not combine if the result of GetMaxSpeed() differs by this much between moves. */
	float MaxSpeedThresholdCombine;

	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear();

	/** Called to set up this saved move (when initially created) to make a predictive correction. */
	virtual void SetMoveFor(AGAVehicle* V, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Vehicle& ClientData);

	/** Set the properties describing the position, etc. of the moved pawn at the start of the move. */
	virtual void SetInitialPosition(AGAVehicle* V);

	/** Returns true if this move is an "important" move that should be sent again if not acked by the server */
	// virtual bool IsImportantMove(const FSavedMovePtr& LastAckedMove) const;

	/** Returns starting position if we were to revert the move, either absolute StartLocation, or StartRelativeLocation offset from MovementBase's current location (since we want to try to move forward at this time). */
	virtual FVector GetRevertedLocation() const;

	enum EPostUpdateMode
	{
		PostUpdate_Record,		// Record a move after having run the simulation
		PostUpdate_Replay,		// Update after replaying a move for a client correction
	};

	/** Set the properties describing the final position, etc. of the moved pawn. */
	virtual void PostUpdate(AGAVehicle* V, EPostUpdateMode PostUpdateMode);

	/** Returns true if this move can be combined with NewMove for replication without changing any behavior */
	// virtual bool CanCombineWith(const FSavedMovePtr& NewMove, AGAVehicle* InVehicle, float MaxDelta) const;

	/** Combine this move with an older move and update relevant state. */
	// virtual void CombineWith(const FSavedMove_Vehicle* OldMove, AGAVehicle* InVehicle, APlayerController* PC, const FVector& OldStartLocation);

	/** Called before ClientUpdatePosition uses this SavedMove to make a predictive correction	 */
	virtual void PrepMoveFor(AGAVehicle* V);

	/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual uint8 GetCompressedFlags() const;

	/** Compare current control rotation with stored starting data */
	virtual bool IsMatchingStartControlRotation(const APlayerController* PC) const;

	/** Packs control rotation for network transport */
	virtual void GetPackedAngles(uint32& YawAndPitchPack, uint8& RollPack) const;

	/** Allows references to be considered during GC */
	virtual void AddStructReferencedObjects(FReferenceCollector& Collector) const;

	// Bit masks used by GetCompressedFlags() to encode movement information.
	enum CompressedFlags
	{
		FLAG_Reserved_0 = 0x01,	// Reserved for future use
		FLAG_Reserved_1 = 0x02,	// Reserved for future use
		FLAG_Reserved_2 = 0x04,	// Reserved for future use
		FLAG_Reserved_3 = 0x08,	// Reserved for future use
		// Remaining bit masks are available for custom flags.
		FLAG_Custom_0 = 0x10,
		FLAG_Custom_1 = 0x20,
		FLAG_Custom_2 = 0x40,
		FLAG_Custom_3 = 0x80,
	};
};

class FVehicleReplaySample
{
public:
	FVehicleReplaySample() : RemoteViewPitch(0), Time(0.0f)
	{
	}

	friend TREASUREDCOVE_API FArchive& operator<<(FArchive& Ar, FVehicleReplaySample& V);

	FVector			Location;
	FRotator		Rotation;
	FVector			Velocity;
	FVector			Acceleration;
	uint8			RemoteViewPitch;
	float			Time;					// This represents time since replay started
};

class TREASUREDCOVE_API FNetworkPredictionData_Client_Vehicle : public FNetworkPredictionData_Client, protected FNoncopyable
{
	using Super = FNetworkPredictionData_Client;

public:

	FNetworkPredictionData_Client_Vehicle(const UAircraftMovementComponent& ClientMovement);
	virtual ~FNetworkPredictionData_Client_Vehicle();

	/** Allows references to be considered during GC */
	void AddStructReferencedObjects(FReferenceCollector& Collector) const override;

	/** Client timestamp of last time it sent a servermove() to the server. This is an increasing timestamp from the owning UWorld. Used for holding off on sending movement updates to save bandwidth. */
	float ClientUpdateTime;

	/** Current TimeStamp for sending new Moves to the Server. This time resets to zero at a frequency of MinTimeBetweenTimeStampResets. */
	float CurrentTimeStamp;

	/** Last World timestamp (undilated, real time) at which we received a server ack for a move. This could be either a good move or a correction from the server. */
	float LastReceivedAckRealTime;

	// TArray<FSavedMovePtr> SavedMoves;		// Buffered moves pending position updates, orderd oldest to newest. Moves that have been acked by the server are removed.
	// TArray<FSavedMovePtr> FreeMoves;		// freed moves, available for buffering
	// FSavedMovePtr PendingMove;				// PendingMove already processed on client - waiting to combine with next movement to reduce client to server bandwidth
	// FSavedMovePtr LastAckedMove;			// Last acknowledged sent move.

	int32 MaxFreeMoveCount;					// Limit on size of free list
	int32 MaxSavedMoveCount;				// Limit on the size of the saved move buffer

	uint32 bUpdatePosition : 1; // when true, update the position (via ClientUpdatePosition)

	// Mesh smoothing variables (for network smoothing)
	//

	/** Used for position smoothing in net games */
	FVector OriginalMeshTranslationOffset;

	/** World space offset of the mesh. Target value is zero offset. Used for position smoothing in net games. */
	FVector MeshTranslationOffset;

	/** Used for rotation smoothing in net games (only used by linear smoothing). */
	FQuat OriginalMeshRotationOffset;

	/** Component space offset of the mesh. Used for rotation smoothing in net games. */
	FQuat MeshRotationOffset;

	/** Target for mesh rotation interpolation. */
	FQuat MeshRotationTarget;

	/** Used for remembering how much time has passed between server corrections */
	float LastCorrectionDelta;

	/** Used to track time of last correction */
	float LastCorrectionTime;

	/** Max time delta between server updates over which client smoothing is allowed to interpolate. */
	float MaxClientSmoothingDeltaTime;

	/** Used to track the timestamp of the last server move. */
	double SmoothingServerTimeStamp;

	/** Used to track the client time as we try to match the server.*/
	double SmoothingClientTimeStamp;

	/**
	 * Copied value from UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance.
	 * @see UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance
	 */
	float MaxSmoothNetUpdateDist;

	/**
	 * Copied value from UCharacterMovementComponent::NetworkNoSmoothUpdateDistance.
	 * @see UCharacterMovementComponent::NetworkNoSmoothUpdateDistance
	 */
	float NoSmoothNetUpdateDist;

	/** How long to take to smoothly interpolate from the old pawn position on the client to the corrected one sent by the server.  Must be >= 0. Not used for linear smoothing. */
	float SmoothNetUpdateTime;

	/** How long to take to smoothly interpolate from the old pawn rotation on the client to the corrected one sent by the server.  Must be >= 0. Not used for linear smoothing. */
	float SmoothNetUpdateRotationTime;

	/**
	 * Max delta time for a given move, in real seconds
	 * Based off of AGameNetworkManager::MaxMoveDeltaTime config setting, but can be modified per actor
	 * if needed.
	 * This value is mirrored in FNetworkPredictionData_Server, which is what server logic runs off of.
	 * Client needs to know this in order to not send move deltas that are going to get clamped anyway (meaning
	 * they'll be rejected/corrected).
	 * Note: This was previously named MaxResponseTime, but has been renamed to reflect what it does more accurately
	 */
	float MaxMoveDeltaTime;

	/** Values used for visualization and debugging of simulated net corrections */
	FVector LastSmoothLocation;
	FVector LastServerLocation;
	float	SimulatedDebugDrawTime;

	/** Array of replay samples that we use to interpolate between to get smooth location/rotation/velocity/ect */
	TArray< FVehicleReplaySample > ReplaySamples;

	/** Finds SavedMove index for given TimeStamp. Returns INDEX_NONE if not found (move has been already Acked or cleared). */
	int32 GetSavedMoveIndex(float TimeStamp) const;

	/** Ack a given move. This move will become LastAckedMove, SavedMoves will be adjusted to only contain unAcked moves. */
	void AckMove(int32 AckedMoveIndex, UAircraftMovementComponent& AircraftMovementComponent);

	/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
	// virtual FSavedMovePtr AllocateNewMove();

	/** Return a move to the free move pool. Assumes that 'Move' will no longer be referenced by anything but possibly the FreeMoves list. Clears PendingMove if 'Move' is PendingMove. */
	// virtual void FreeMove(const FSavedMovePtr& Move);

	/** Tries to pull a pooled move off the free move list, otherwise allocates a new move. Returns NULL if the limit on saves moves is hit. */
	// virtual FSavedMovePtr CreateSavedMove();

	/** Update CurentTimeStamp from passed in DeltaTime.
		It will measure the accuracy between passed in DeltaTime and how Server will calculate its DeltaTime.
		If inaccuracy is too high, it will reset CurrentTimeStamp to maintain a high level of accuracy.
		@return DeltaTime to use for Client's physics simulation prior to replicate move to server. */
	float UpdateTimeStampAndDeltaTime(float DeltaTime, AGAVehicle& VehicleOwner, class UAircraftMovementComponent& AircraftMovementComponent);

	/** Used for simulated packet loss in development builds. */
	float DebugForcedPacketLossTimerStart;
};

class TREASUREDCOVE_API FNetworkPredictionData_Server_Vehicle : public FNetworkPredictionData_Server, protected FNoncopyable
{
public:

	FNetworkPredictionData_Server_Vehicle(const UAircraftMovementComponent& ServerMovement);
	virtual ~FNetworkPredictionData_Server_Vehicle();

	FClientAdjustment PendingAdjustment;

	/** Timestamp from the client of most recent ServerMove() processed for this player. Reset occasionally for timestamp resets (to maintain accuracy). */
	float CurrentClientTimeStamp;

	/** Timestamp from the client of most recent ServerMove() received for this player, including rejected requests. */
	float LastReceivedClientTimeStamp;

	/** Timestamp of total elapsed client time. Similar to CurrentClientTimestamp but this is accumulated with the calculated DeltaTime for each move on the server. */
	double ServerAccumulatedClientTimeStamp;

	/** Last time server updated client with a move correction */
	float LastUpdateTime;

	/** Server clock time when last server move was received from client (does NOT include forced moves on server) */
	float ServerTimeStampLastServerMove;

	/**
	 * Max delta time for a given move, in real seconds
	 * Based off of AGameNetworkManager::MaxMoveDeltaTime config setting, but can be modified per actor
	 * if needed.
	 * Note: This was previously named MaxResponseTime, but has been renamed to reflect what it does more accurately
	 */
	float MaxMoveDeltaTime;

	/** Force client update on the next ServerMoveHandleClientError() call. */
	uint32 bForceClientUpdate : 1;

	/** Accumulated timestamp difference between autonomous client and server for tracking long-term trends */
	float LifetimeRawTimeDiscrepancy;

	/**
	 * Current time discrepancy between client-reported moves and time passed
	 * on the server. Time discrepancy resolution's goal is to keep this near zero.
	 */
	float TimeDiscrepancy;

	/** True if currently in the process of resolving time discrepancy */
	bool bResolvingTimeDiscrepancy;

	/**
	 * When bResolvingTimeDiscrepancy is true, we are in time discrepancy resolution mode whose output is
	 * this value (to be used as the DeltaTime for current ServerMove)
	 */
	float TimeDiscrepancyResolutionMoveDeltaOverride;

	/**
	 * When bResolvingTimeDiscrepancy is true, we are in time discrepancy resolution mode where we bound
	 * move deltas by Server Deltas. In cases where there are multiple ServerMove RPCs handled within one
	 * server frame tick, we need to accumulate the client deltas of the "no tick" Moves so that the next
	 * Move processed that the server server has ticked for takes into account those previous deltas.
	 * If we did not use this, the higher the framerate of a client vs the server results in more
	 * punishment/payback time.
	 */
	float TimeDiscrepancyAccumulatedClientDeltasSinceLastServerTick;

	/** Creation time of this prediction data, used to contextualize LifetimeRawTimeDiscrepancy */
	float WorldCreationTime;

	/** Returns time delta to use for the current ServerMove(). Takes into account time discrepancy resolution if active. */
	float GetServerMoveDeltaTime(float ClientTimeStamp, float ActorTimeDilation) const;

	/** Returns base time delta to use for a ServerMove, default calculation (no time discrepancy resolution) */
	float GetBaseServerMoveDeltaTime(float ClientTimeStamp, float ActorTimeDilation) const;

};


