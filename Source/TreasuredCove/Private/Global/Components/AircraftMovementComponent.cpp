// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/AircraftMovementComponent.h"
#include "GAVehicle.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/GameNetworkManager.h"
#include "Engine/NetworkObjectList.h"
#include "Engine/NetSerialization.h"
#include "Engine/NetDriver.h"


// #include UE_INLINE_GENERATED_CPP_BY_NAME(VehicleMovementComponent)

CSV_DEFINE_CATEGORY(VehicleMovement, true);

DEFINE_LOG_CATEGORY_STATIC(LogVehicleMovement, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogNavMeshMovement, Log, All);
DEFINE_LOG_CATEGORY_STATIC(LogVehicleNetSmoothing, Log, All);

// CVars
namespace VehicleMovementCVars
{
	// Use newer RPCs and RPC parameter serialization that allow variable length data without changing engine APIs.
	static int32 NetUsePackedMovementRPCs = 1;
	FAutoConsoleVariableRef CVarNetUsePackedMovementRPCs(
		TEXT("p.NetUsePackedMovementRPCs"),
		NetUsePackedMovementRPCs,
		TEXT("Whether to use newer movement RPC parameter packed serialization. If disabled, old deprecated movement RPCs will be used instead.\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);

	static int32 NetPackedMovementMaxBits = 4096;
	FAutoConsoleVariableRef CVarNetPackedMovementMaxBits(
		TEXT("p.NetPackedMovementMaxBits"),
		NetPackedMovementMaxBits,
		TEXT("Max number of bits allowed in each packed movement RPC. Used to protect against bad data causing the server to allocate too much memory.\n"),
		ECVF_Default);

	// Listen server smoothing
	static int32 NetEnableListenServerSmoothing = 1;
	FAutoConsoleVariableRef CVarNetEnableListenServerSmoothing(
		TEXT("p.NetEnableListenServerSmoothing"),
		NetEnableListenServerSmoothing,
		TEXT("Whether to enable mesh smoothing on listen servers for the local view of remote clients.\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);

	// Latent proxy prediction
	static int32 NetEnableSkipProxyPredictionOnNetUpdate = 1;
	FAutoConsoleVariableRef CVarNetEnableSkipProxyPredictionOnNetUpdate(
		TEXT("p.NetEnableSkipProxyPredictionOnNetUpdate"),
		NetEnableSkipProxyPredictionOnNetUpdate,
		TEXT("Whether to allow proxies to skip prediction on frames with a network position update, if bNetworkSkipProxyPredictionOnNetUpdate is also true on the movement component.\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);

	static int32 EnableQueuedAnimEventsOnServer = 0;
	FAutoConsoleVariableRef CVarEnableQueuedAnimEventsOnServer(
		TEXT("a.EnableQueuedAnimEventsOnServer"),
		EnableQueuedAnimEventsOnServer,
		TEXT("Whether to enable queued anim events on servers. In most cases, when the server is doing a full anim graph update, queued notifies aren't triggered by the server, but this will enable them. Enabling this is recommended in projects using Listen Servers. \n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);

	// Logging when Vehicle is stuck. Off by default in shipping.
#if UE_BUILD_SHIPPING
	static float StuckWarningPeriod = -1.f;
#else
	static float StuckWarningPeriod = 1.f;
#endif

	FAutoConsoleVariableRef CVarStuckWarningPeriod(
		TEXT("p.VehicleStuckWarningPeriod"),
		StuckWarningPeriod,
		TEXT("How often (in seconds) we are allowed to log a message about being stuck in geometry.\n")
		TEXT("<0: Disable, >=0: Enable and log this often, in seconds."),
		ECVF_Default);

	static int32 NetEnableMoveCombining = 1;
	FAutoConsoleVariableRef CVarNetEnableMoveCombining(
		TEXT("p.NetEnableMoveCombining"),
		NetEnableMoveCombining,
		TEXT("Whether to enable move combining on the client to reduce bandwidth by combining similar moves.\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);

	static int32 NetEnableMoveCombiningOnStaticBaseChange = 1;
	FAutoConsoleVariableRef CVarNetEnableMoveCombiningOnStaticBaseChange(
		TEXT("p.NetEnableMoveCombiningOnStaticBaseChange"),
		NetEnableMoveCombiningOnStaticBaseChange,
		TEXT("Whether to allow combining client moves when moving between static geometry.\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);

	static float NetMoveCombiningAttachedLocationTolerance = 0.01f;
	FAutoConsoleVariableRef CVarNetMoveCombiningAttachedLocationTolerance(
		TEXT("p.NetMoveCombiningAttachedLocationTolerance"),
		NetMoveCombiningAttachedLocationTolerance,
		TEXT("Tolerance for relative location attachment change when combining moves. Small tolerances allow for very slight jitter due to transform updates."),
		ECVF_Default);

	static float NetMoveCombiningAttachedRotationTolerance = 0.01f;
	FAutoConsoleVariableRef CVarNetMoveCombiningAttachedRotationTolerance(
		TEXT("p.NetMoveCombiningAttachedRotationTolerance"),
		NetMoveCombiningAttachedRotationTolerance,
		TEXT("Tolerance for relative rotation attachment change when combining moves. Small tolerances allow for very slight jitter due to transform updates."),
		ECVF_Default);

	static float NetStationaryRotationTolerance = 0.1f;
	FAutoConsoleVariableRef CVarNetStationaryRotationTolerance(
		TEXT("p.NetStationaryRotationTolerance"),
		NetStationaryRotationTolerance,
		TEXT("Tolerance for GetClientNetSendDeltaTime() to remain throttled when small control rotation changes occur."),
		ECVF_Default);

	static int32 NetUseClientTimestampForReplicatedTransform = 1;
	FAutoConsoleVariableRef CVarNetUseClientTimestampForReplicatedTransform(
		TEXT("p.NetUseClientTimestampForReplicatedTransform"),
		NetUseClientTimestampForReplicatedTransform,
		TEXT("If enabled, use client timestamp changes to track the replicated transform timestamp, otherwise uses server tick time as the timestamp.\n")
		TEXT("Game session usually needs to be restarted if this is changed at runtime.\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Default);


	static int32 ReplayLerpAcceleration = 0;
	FAutoConsoleVariableRef CVarReplayLerpAcceleration(
		TEXT("p.ReplayLerpAcceleration"),
		ReplayLerpAcceleration,
		TEXT(""),
		ECVF_Default);

	static int32 FixReplayOverSampling = 1;
	FAutoConsoleVariableRef CVarFixReplayOverSampling(
		TEXT("p.FixReplayOverSampling"),
		FixReplayOverSampling,
		TEXT("If 1, remove invalid replay samples that can occur due to oversampling (sampling at higher rate than physics is being ticked)"),
		ECVF_Default);

	int32 ForceJumpPeakSubstep = 1;
	FAutoConsoleVariableRef CVarForceJumpPeakSubstep(
		TEXT("p.ForceJumpPeakSubstep"),
		ForceJumpPeakSubstep,
		TEXT("If 1, force a jump substep to always reach the peak position of a jump, which can often be cut off as framerate lowers."),
		ECVF_Default);

	static float NetServerMoveTimestampExpiredWarningThreshold = 1.0f;
	FAutoConsoleVariableRef CVarNetServerMoveTimestampExpiredWarningThreshold(
		TEXT("net.NetServerMoveTimestampExpiredWarningThreshold"),
		NetServerMoveTimestampExpiredWarningThreshold,
		TEXT("Tolerance for ServerMove() to warn when client moves are expired more than this time threshold behind the server."),
		ECVF_Default);

	int32 AsyncVehicleMovement = 0;
	FAutoConsoleVariableRef CVarAsyncVehicleMovement(
		TEXT("p.AsyncVehicleMovement"),
		AsyncVehicleMovement, TEXT("1 enables asynchronous simulation of Vehicle movement on physics thread. Toggling this at runtime is not recommended."));

	int32 BasedMovementMode = 2;
	FAutoConsoleVariableRef CVarBasedMovementMode(
		TEXT("p.BasedMovementMode"),
		BasedMovementMode, TEXT("0 means always on regular tick (default); 1 means only if not deferring updates; 2 means update and save based movement both on regular ticks and post physics when on a physics base."));

	static int32 UseTargetVelocityOnImpact = 1;
	FAutoConsoleVariableRef CVarUseTargetVelocityOnImpact(
		TEXT("p.UseTargetVelocityOnImpact"),
		UseTargetVelocityOnImpact, TEXT("When disabled, we recalculate velocity after impact by comparing our position before we moved to our position after we moved. This doesn't work correctly when colliding with physics objects, so setting this to 1 fixes this one the hit object is moving."));

	static float ClientAuthorityThresholdOnBaseChange = 0.f;
	FAutoConsoleVariableRef CVarClientAuthorityThresholdOnBaseChange(
		TEXT("p.ClientAuthorityThresholdOnBaseChange"),
		ClientAuthorityThresholdOnBaseChange,
		TEXT("When a pawn moves onto or off of a moving base, this can cause an abrupt correction. In these cases, trust the client up to this distance away from the server component location."),
		ECVF_Default);

	static float MaxFallingCorrectionLeash = 0.f;
	FAutoConsoleVariableRef CVarMaxFallingCorrectionLeash(
		TEXT("p.MaxFallingCorrectionLeash"),
		MaxFallingCorrectionLeash,
		TEXT("When airborne, some distance between the server and client locations may remain to avoid sudden corrections as clients jump from moving bases. This value is the maximum allowed distance."),
		ECVF_Default);

	static float MaxFallingCorrectionLeashBuffer = 10.f;
	FAutoConsoleVariableRef CVarMaxFallingCorrectionLeashBuffer(
		TEXT("p.MaxFallingCorrectionLeashBuffer"),
		MaxFallingCorrectionLeashBuffer,
		TEXT("To avoid constant corrections, when an airborne server and client are further than p.MaxFallingCorrectionLeash cm apart, they'll be pulled in to that distance minus this value."),
		ECVF_Default);

	static bool bAddFormerBaseVelocityToRootMotionOverrideWhenFalling = true;
	FAutoConsoleVariableRef CVarAddFormerBaseVelocityToRootMotionOverrideWhenFalling(
		TEXT("p.AddFormerBaseVelocityToRootMotionOverrideWhenFalling"),
		bAddFormerBaseVelocityToRootMotionOverrideWhenFalling,
		TEXT("To avoid sudden velocity changes when a root motion source moves the pawn from a moving base to free fall, this CVar will enable the FormerBaseVelocityDecayHalfLife property on VehicleMovementComponent."),
		ECVF_Default);

	static bool bGeometryCollectionImpulseWorkAround = true;
	FAutoConsoleVariableRef CVarGeometryCollectionImpulseWorkAround(
		TEXT("p.CVarGeometryCollectionImpulseWorkAround"),
		bGeometryCollectionImpulseWorkAround,
		TEXT("This enabled a workaround to allow impulses to be applied to geometry collection.\n"),
		ECVF_Default);

#if !UE_BUILD_SHIPPING

	int32 NetShowCorrections = 0;
	FAutoConsoleVariableRef CVarNetShowCorrections(
		TEXT("p.NetShowCorrections"),
		NetShowCorrections,
		TEXT("Whether to draw client position corrections (red is incorrect, green is corrected).\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Cheat);

	float NetCorrectionLifetime = 4.f;
	FAutoConsoleVariableRef CVarNetCorrectionLifetime(
		TEXT("p.NetCorrectionLifetime"),
		NetCorrectionLifetime,
		TEXT("How long a visualized network correction persists.\n")
		TEXT("Time in seconds each visualized network correction persists."),
		ECVF_Cheat);

#endif // !UE_BUILD_SHIPPING


#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	static float NetForceClientAdjustmentPercent = 0.f;
	FAutoConsoleVariableRef CVarNetForceClientAdjustmentPercent(
		TEXT("p.NetForceClientAdjustmentPercent"),
		NetForceClientAdjustmentPercent,
		TEXT("Percent of ServerCheckClientError checks to return true regardless of actual error.\n")
		TEXT("Useful for testing client correction code.\n")
		TEXT("<=0: Disable, 0.05: 5% of checks will return failed, 1.0: Always send client adjustments"),
		ECVF_Cheat);

	static float NetForceClientServerMoveLossPercent = 0.f;
	FAutoConsoleVariableRef CVarNetForceClientServerMoveLossPercent(
		TEXT("p.NetForceClientServerMoveLossPercent"),
		NetForceClientServerMoveLossPercent,
		TEXT("Percent of ServerMove calls for client to not send.\n")
		TEXT("Useful for testing server force correction code.\n")
		TEXT("<=0: Disable, 0.05: 5% of checks will return failed, 1.0: never send server moves"),
		ECVF_Cheat);

	static float NetForceClientServerMoveLossDuration = 0.f;
	FAutoConsoleVariableRef CVarNetForceClientServerMoveLossDuration(
		TEXT("p.NetForceClientServerMoveLossDuration"),
		NetForceClientServerMoveLossDuration,
		TEXT("Duration in seconds for client to drop ServerMove calls when NetForceClientServerMoveLossPercent check passes.\n")
		TEXT("Useful for testing server force correction code.\n")
		TEXT("Duration of zero means single frame loss."),
		ECVF_Cheat);

	static int32 VisualizeMovement = 0;
	FAutoConsoleVariableRef CVarVisualizeMovement(
		TEXT("p.VisualizeMovement"),
		VisualizeMovement,
		TEXT("Whether to draw in-world debug information for Vehicle movement.\n")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Cheat);

	static int32 NetVisualizeSimulatedCorrections = 0;
	FAutoConsoleVariableRef CVarNetVisualizeSimulatedCorrections(
		TEXT("p.NetVisualizeSimulatedCorrections"),
		NetVisualizeSimulatedCorrections,
		TEXT("")
		TEXT("0: Disable, 1: Enable"),
		ECVF_Cheat);

	static int32 DebugTimeDiscrepancy = 0;
	FAutoConsoleVariableRef CVarDebugTimeDiscrepancy(
		TEXT("p.DebugTimeDiscrepancy"),
		DebugTimeDiscrepancy,
		TEXT("Whether to log detailed Movement Time Discrepancy values for testing")
		TEXT("0: Disable, 1: Enable Detection logging, 2: Enable Detection and Resolution logging"),
		ECVF_Cheat);
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
}


UAircraftMovementComponent::UAircraftMovementComponent()
{
	MaxSimulationTimeStep = 0.05f;
	MaxSimulationIterations = 8;

	ClientPredictionData = NULL;
	ServerPredictionData = NULL;
	SetNetworkMoveDataContainer(DefaultNetworkMoveDataContainer);
	// SetMoveResponseDataContainer(DefaultMoveResponseDataContainer);
	ServerMoveBitWriter.SetAllowResize(true);
	MoveResponseBitWriter.SetAllowResize(true);

	// This should be greater than tolerated player timeout * 2.
	MinTimeBetweenTimeStampResets = 4.f * 60.f;
	LastTimeStampResetServerTime = 0.f;

	bEnableScopedMovementUpdates = true;
}


const float UAircraftMovementComponent::MIN_TICK_TIME = 1e-6f;

FNetworkPredictionData_Client* UAircraftMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UAircraftMovementComponent* MutableThis = const_cast<UAircraftMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Vehicle(*this);
	}

	return ClientPredictionData;
}

FNetworkPredictionData_Server* UAircraftMovementComponent::GetPredictionData_Server() const
{
	if (ServerPredictionData == nullptr)
	{
		UAircraftMovementComponent* MutableThis = const_cast<UAircraftMovementComponent*>(this);
		MutableThis->ServerPredictionData = new FNetworkPredictionData_Server_Vehicle(*this);
	}

	return ServerPredictionData;
}

FNetworkPredictionData_Client_Vehicle* UAircraftMovementComponent::GetPredictionData_Client_Vehicle() const
{
	// Should only be called on client or listen server (for remote clients) in network games
	checkSlow(VehicleOwner != NULL);
	checkSlow(VehicleOwner->GetLocalRole() < ROLE_Authority || (VehicleOwner->GetRemoteRole() == ROLE_AutonomousProxy && GetNetMode() == NM_ListenServer));
	checkSlow(GetNetMode() == NM_Client || GetNetMode() == NM_ListenServer);

	if (ClientPredictionData == nullptr)
	{
		UAircraftMovementComponent* MutableThis = const_cast<UAircraftMovementComponent*>(this);
		MutableThis->ClientPredictionData = static_cast<class FNetworkPredictionData_Client_Vehicle*>(GetPredictionData_Client());
	}

	return ClientPredictionData;
}

FNetworkPredictionData_Server_Vehicle* UAircraftMovementComponent::GetPredictionData_Server_Vehicle() const
{
	// Should only be called on server in network games
	checkSlow(VehicleOwner != NULL);
	checkSlow(VehicleOwner->GetLocalRole() == ROLE_Authority);
	checkSlow(GetNetMode() < NM_Client);

	if (ServerPredictionData == nullptr)
	{
		UAircraftMovementComponent* MutableThis = const_cast<UAircraftMovementComponent*>(this);
		MutableThis->ServerPredictionData = static_cast<class FNetworkPredictionData_Server_Vehicle*>(GetPredictionData_Server());
	}

	return ServerPredictionData;
}

bool UAircraftMovementComponent::HasPredictionData_Client() const
{
	return (ClientPredictionData != nullptr) && HasValidData();
}

bool UAircraftMovementComponent::HasPredictionData_Server() const
{
	return (ServerPredictionData != nullptr) && HasValidData();
}

void UAircraftMovementComponent::ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration)
{
	//	SCOPE_CYCLE_COUNTER(STAT_VehicleMovementReplicateMoveToServer);
	check(VehicleOwner != NULL);

	// Can only start sending moves if our controllers are synced up over the network, otherwise we flood the reliable buffer.
	APlayerController* PC = Cast<APlayerController>(VehicleOwner->GetController());
	if (PC && PC->AcknowledgedPawn != VehicleOwner)
	{
		return;
	}

	// Bail out if our Vehicle's controller doesn't have a Player. This may be the case when the local player
	// has switched to another controller, such as a debug camera controller.
	if (PC && PC->Player == nullptr)
	{
		return;
	}

	FNetworkPredictionData_Client_Vehicle* ClientData = GetPredictionData_Client_Vehicle();
	if (!ClientData)
	{
		return;
	}

	// Update our delta time for physics simulation.
	DeltaTime = ClientData->UpdateTimeStampAndDeltaTime(DeltaTime, *VehicleOwner, *this);

	// Find the oldest (unacknowledged) important move (OldMove).
	// Don't include the last move because it may be combined with the next new move.
	// A saved move is interesting if it differs significantly from the last acknowledged move
	
	// FSavedMovePtr OldMove = NULL;
	// if (ClientData->LastAckedMove.IsValid())
	{
		const int32 NumSavedMoves = 0; // = ClientData->SavedMoves.Num();
		for (int32 i = 0; i < NumSavedMoves - 1; i++)
		{
			// const FSavedMovePtr& CurrentMove = ClientData->SavedMoves[i];
			// if (CurrentMove->IsImportantMove(ClientData->LastAckedMove))
			{
				// OldMove = CurrentMove;
				break;
			}
		}
	}

	//	// Get a SavedMove object to store the movement in.
	// FSavedMovePtr NewMovePtr = ClientData->CreateSavedMove();
	FSavedMove_Vehicle* const NewMove = nullptr; // = NewMovePtr.Get();
	if (NewMove == nullptr)
	{
		return;
	}

	NewMove->SetMoveFor(VehicleOwner, DeltaTime, NewAcceleration, *ClientData);
	const UWorld* MyWorld = GetWorld();

	// see if the two moves could be combined
	// do not combine moves which have different TimeStamps (before and after reset).
	if (const FSavedMove_Vehicle* PendingMove = nullptr)// ClientData->PendingMove.Get())
	{
		// if (PendingMove->CanCombineWith(NewMovePtr, VehicleOwner, ClientData->MaxMoveDeltaTime * VehicleOwner->GetActorTimeDilation(*MyWorld)))
		{
			// SCOPE_CYCLE_COUNTER(STAT_VehicleMovementCombineNetMove);

			// Only combine and move back to the start location if we don't move back in to a spot that would make us collide with something new.
			const FVector OldStartLocation = PendingMove->GetRevertedLocation();
			// const bool bAttachedToObject = (NewMovePtr->StartAttachParent != nullptr);
			// if (bAttachedToObject || !OverlapTest(OldStartLocation, PendingMove->StartRotation.Quaternion(), UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), VehicleOwner))
			{
				// Avoid updating Mesh bones to physics during the teleport back, since PerformMovement() will update it right away anyway below.
				// Note: this must be before the FScopedMovementUpdate below, since that scope is what actually moves the Vehicle and mesh.
				// FScopedMeshBoneUpdateOverride ScopedNoMeshBoneUpdate(VehicleOwner->GetMesh(), EKinematicBonesUpdateToPhysics::SkipAllBones);

				// Accumulate multiple transform updates until scope ends.
				FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, EScopedUpdate::DeferredUpdates);
				UE_LOG(LogNetPlayerMovement, VeryVerbose, TEXT("CombineMove: add delta %f + %f and revert from %f %f to %f %f"), DeltaTime, PendingMove->DeltaTime, UpdatedComponent->GetComponentLocation().X, UpdatedComponent->GetComponentLocation().Y, OldStartLocation.X, OldStartLocation.Y);

				// NewMove->CombineWith(PendingMove, VehicleOwner, PC, OldStartLocation);

				if (PC)
				{
					// We reverted position to that at the start of the pending move (above), however some code paths expect rotation to be set correctly
					// before Vehicle movement occurs (via FaceRotation), so try that now. The bOrientRotationToMovement path happens later as part of PerformMovement() and PhysicsRotation().
					// VehicleOwner->FaceRotation(PC->GetControlRotation(), NewMove->DeltaTime);
				}

				// SaveBaseLocation();
				NewMove->SetInitialPosition(VehicleOwner);

				// Remove pending move from move list. It would have to be the last move on the list.
				// if (ClientData->SavedMoves.Num() > 0 && ClientData->SavedMoves.Last() == ClientData->PendingMove)
				{
					const bool bAllowShrinking = false;
					// ClientData->SavedMoves.Pop(bAllowShrinking);
				}
				// ClientData->FreeMove(ClientData->PendingMove);
				// ClientData->PendingMove = nullptr;
				PendingMove = nullptr; // Avoid dangling reference, it's deleted above.
			}
			// else
			{
				UE_LOG(LogNetPlayerMovement, Verbose, TEXT("Not combining move [would collide at start location]"));
			}
		}
		// else
		{
			UE_LOG(LogNetPlayerMovement, Verbose, TEXT("Not combining move [not allowed by CanCombineWith()]"));
		}
	}

	//	// Acceleration should match what we send to the server, plus any other restrictions the server also enforces (see MoveAutonomous).
	//	Acceleration = NewMove->Acceleration.GetClampedToMaxSize(GetMaxAcceleration());
	//	AnalogInputModifier = ComputeAnalogInputModifier(); // recompute since acceleration may have changed.
	//
	//	// Perform the move locally
	//	VehicleOwner->ClientRootMotionParams.Clear();
	//	VehicleOwner->SavedRootMotion.Clear();
	PerformMovement(NewMove->DeltaTime);

	NewMove->PostUpdate(VehicleOwner, FSavedMove_Vehicle::PostUpdate_Record);

	// Add NewMove to the list
	if (VehicleOwner->IsReplicatingMovement())
	{
		// check(NewMove == NewMovePtr.Get());
		// ClientData->SavedMoves.Push(NewMovePtr);

		//		const bool bCanDelayMove = (VehicleMovementCVars::NetEnableMoveCombining != 0) && CanDelaySendingMove(NewMovePtr);
		//
		//		if (bCanDelayMove && ClientData->PendingMove.IsValid() == false)
		//		{
		//			// Decide whether to hold off on move
		//			const float NetMoveDelta = FMath::Clamp(GetClientNetSendDeltaTime(PC, ClientData, NewMovePtr), 1.f / 120.f, 1.f / 5.f);
		//
		//			if ((MyWorld->TimeTime - ClientData->ClientUpdateTime) * MyWorld->GetWorldSettings()->GetEffectiveTimeDilation() < NetMoveDelta)
		//			{
		//				// Delay sending this move.
		//				ClientData->PendingMove = NewMovePtr;
		//				return;
		//			}
		//		}

		// ClientData->ClientUpdateTime = MyWorld->TimeTime;

		//		UE_CLOG(VehicleOwner && UpdatedComponent, LogNetPlayerMovement, VeryVerbose, TEXT("ClientMove Time %f Acceleration %s Velocity %s Position %s Rotation %s DeltaTime %f Mode %s MovementBase %s.%s (Dynamic:%d) DualMove? %d"),
		//			NewMove->TimeStamp, *NewMove->Acceleration.ToString(), *Velocity.ToString(), *UpdatedComponent->GetComponentLocation().ToString(), *UpdatedComponent->GetComponentRotation().ToCompactString(), NewMove->DeltaTime, *GetMovementName(),
		//			*GetNameSafe(NewMove->EndBase.Get()), *NewMove->EndBoneName.ToString(), MovementBaseUtility::IsDynamicBase(NewMove->EndBase.Get()) ? 1 : 0, ClientData->PendingMove.IsValid() ? 1 : 0);


		bool bSendServerMove = true;

		//#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		//		// Testing options: Simulated packet loss to server
		//		const float TimeSinceLossStart = (MyWorld->RealTimeTime - ClientData->DebugForcedPacketLossTimerStart);
		//		if (ClientData->DebugForcedPacketLossTimerStart > 0.f && (TimeSinceLossStart < VehicleMovementCVars::NetForceClientServerMoveLossDuration))
		//		{
		//			bSendServerMove = false;
		//			UE_LOG(LogNetPlayerMovement, Log, TEXT("Drop ServerMove, %.2f time remains"), VehicleMovementCVars::NetForceClientServerMoveLossDuration - TimeSinceLossStart);
		//		}
		//		else if (VehicleMovementCVars::NetForceClientServerMoveLossPercent != 0.f && (RandomStream.FRand() < VehicleMovementCVars::NetForceClientServerMoveLossPercent))
		//		{
		//			bSendServerMove = false;
		//			ClientData->DebugForcedPacketLossTimerStart = (VehicleMovementCVars::NetForceClientServerMoveLossDuration > 0) ? MyWorld->RealTimeTime : 0.0f;
		//			UE_LOG(LogNetPlayerMovement, Log, TEXT("Drop ServerMove, %.2f time remains"), VehicleMovementCVars::NetForceClientServerMoveLossDuration);
		//		}
		//		else
		//		{
		//			ClientData->DebugForcedPacketLossTimerStart = 0.f;
		//		}
		//#endif

				// Send move to server if this Vehicle is replicating movement
		if (bSendServerMove)
		{
			// CallServerMovePacked(NewMove, ClientData->PendingMove.Get(), OldMove.Get());
		}
	}

	// ClientData->PendingMove = NULL;
}

void UAircraftMovementComponent::CallServerMovePacked(const FSavedMove_Vehicle* NewMove, const FSavedMove_Vehicle* PendingMove, const FSavedMove_Vehicle* OldMove)
{
	// Get storage container we'll be using and fill it with movement data
	FVehicleNetworkMoveDataContainer& MoveDataContainer = GetNetworkMoveDataContainer();
	MoveDataContainer.ClientFillNetworkMoveData(NewMove, PendingMove, OldMove);

	// Reset bit writer without affecting allocations
	FBitWriterMark BitWriterReset;
	BitWriterReset.Pop(ServerMoveBitWriter);

	// 'static' to avoid reallocation each invocation
	static FVehicleServerMovePackedBits PackedBits;
	UNetConnection* NetConnection = VehicleOwner->GetNetConnection();

#if UE_WITH_IRIS
	if (UPackageMap* PackageMap = nullptr/*UE::Private::GetIrisPackageMapToCaptureReferences(NetConnection, &PackedBits.ObjectReferences)*/)
	{
		ServerMoveBitWriter.PackageMap = PackageMap;
	}
	else
#endif
	{
		// Extract the net package map used for serializing object references.
		ServerMoveBitWriter.PackageMap = NetConnection ? ToRawPtr(NetConnection->PackageMap) : nullptr;
	}

	if (ServerMoveBitWriter.PackageMap == nullptr)
	{
		UE_LOG(LogNetPlayerMovement, Error, TEXT("CallServerMovePacked: Failed to find a NetConnection/PackageMap for data serialization!"));
		return;
	}

	// Serialize move struct into a bit stream
	if (!MoveDataContainer.Serialize(*this, ServerMoveBitWriter, ServerMoveBitWriter.PackageMap) || ServerMoveBitWriter.IsError())
	{
		UE_LOG(LogNetPlayerMovement, Error, TEXT("CallServerMovePacked: Failed to serialize out movement data!"));
		return;
	}

	// Copy bits to our struct that we can NetSerialize to the server.
	PackedBits.DataBits.SetNumUninitialized(ServerMoveBitWriter.GetNumBits());

	check(PackedBits.DataBits.Num() >= ServerMoveBitWriter.GetNumBits());
	FMemory::Memcpy(PackedBits.DataBits.GetData(), ServerMoveBitWriter.GetData(), ServerMoveBitWriter.GetNumBytes());

	// Send bits to server!
	ServerMovePacked_ClientSend(PackedBits);

	MarkForClientCameraUpdate();
}

void UAircraftMovementComponent::ServerMovePacked_ClientSend(const FVehicleServerMovePackedBits& PackedBits)
{
	// Pass through RPC call to Vehicle on server, there is less RPC bandwidth overhead when used on an Actor rather than a Component.
	VehicleOwner->ServerMovePacked(PackedBits);
}

void UAircraftMovementComponent::ServerMovePacked_ServerReceive(const FVehicleServerMovePackedBits& PackedBits)
{
	if (!HasValidData() || !IsActive())
	{
		return;
	}

	const int32 NumBits = PackedBits.DataBits.Num();
	if (!ensureMsgf(NumBits <= VehicleMovementCVars::NetPackedMovementMaxBits, TEXT("ServerMovePacked_ServerReceive: NumBits (%d) exceeds VehicleMovementCVars::NetPackedMovementMaxBits (%d)"), NumBits, VehicleMovementCVars::NetPackedMovementMaxBits))
	{
		// Protect against bad data that could cause server to allocate way too much memory.
		// devCode(UE_LOG(LogNetPlayerMovement, Error, TEXT("ServerMovePacked_ServerReceive: NumBits (%d) exceeds allowable limit!"), NumBits));
		return;
	}

	// Reuse bit reader to avoid allocating memory each time.
	ServerMoveBitReader.SetData((uint8*)PackedBits.DataBits.GetData(), NumBits);

#if UE_WITH_IRIS
	if (UPackageMap* PackageMap = nullptr/*UE::Private::GetIrisPackageMapToReadReferences(VehicleOwner->GetNetConnection(), &PackedBits.ObjectReferences)*/)
	{
		ServerMoveBitReader.PackageMap = PackageMap;
	}
	else
#endif
	{
		ServerMoveBitReader.PackageMap = PackedBits.GetPackageMap();
	}

	// Deserialize bits to move data struct.
	// We had to wait until now and use the temp bit stream because the RPC doesn't know about the virtual overrides on the possibly custom struct that is our data container.
	FVehicleNetworkMoveDataContainer& MoveDataContainer = GetNetworkMoveDataContainer();
	if (!MoveDataContainer.Serialize(*this, ServerMoveBitReader, ServerMoveBitReader.PackageMap) || ServerMoveBitReader.IsError())
	{
		// devCode(UE_LOG(LogNetPlayerMovement, Error, TEXT("ServerMovePacked_ServerReceive: Failed to serialize movement data!")));
		return;
	}

	ServerMove_HandleMoveData(MoveDataContainer);
}

void UAircraftMovementComponent::ServerMove_HandleMoveData(const FVehicleNetworkMoveDataContainer& MoveDataContainer)
{
	// Optional "old move"
	if (MoveDataContainer.bHasOldMove)
	{
		if (FVehicleNetworkMoveData* OldMove = MoveDataContainer.GetOldMoveData())
		{
			SetCurrentNetworkMoveData(OldMove);
			ServerMove_PerformMovement(*OldMove);
		}
	}

	// Optional scoped movement update for dual moves to combine moves for cheaper performance on the server.
	const bool bMoveAllowsScopedDualMove = MoveDataContainer.bHasPendingMove && !MoveDataContainer.bDisableCombinedScopedMove;
	FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, (bMoveAllowsScopedDualMove && bEnableServerDualMoveScopedMovementUpdates && bEnableScopedMovementUpdates) ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);

	// Optional pending move as part of "dual move"
	if (MoveDataContainer.bHasPendingMove)
	{
		if (FVehicleNetworkMoveData* PendingMove = MoveDataContainer.GetPendingMoveData())
		{
			// VehicleOwner->bServerMoveIgnoreRootMotion = MoveDataContainer.bIsDualHybridRootMotionMove && VehicleOwner->IsPlayingNetworkedRootMotionMontage();
			SetCurrentNetworkMoveData(PendingMove);
			ServerMove_PerformMovement(*PendingMove);
			// VehicleOwner->bServerMoveIgnoreRootMotion = false;
		}
	}

	// Final standard move
	if (FVehicleNetworkMoveData* NewMove = MoveDataContainer.GetNewMoveData())
	{
		SetCurrentNetworkMoveData(NewMove);
		ServerMove_PerformMovement(*NewMove);
	}

	SetCurrentNetworkMoveData(nullptr);
}

void UAircraftMovementComponent::ServerMove_PerformMovement(const FVehicleNetworkMoveData& MoveData)
{
	// SCOPE_CYCLE_COUNTER(STAT_VehicleMovementServerMove);
	// CSV_SCOPED_TIMING_STAT(VehicleMovement, VehicleMovementServerMove);

	if (!HasValidData() || !IsActive())
	{
		return;
	}

	const float ClientTimeStamp = MoveData.TimeStamp;
	FVector_NetQuantize10 ClientAccel = MoveData.Acceleration;
	const uint8 ClientMoveFlags = MoveData.CompressedMoveFlags;
	const FRotator ClientControlRotation = MoveData.ControlRotation;

	FNetworkPredictionData_Server_Vehicle* ServerData = GetPredictionData_Server_Vehicle();
	check(ServerData);

	if (!VerifyClientTimeStamp(ClientTimeStamp, *ServerData))
	{
		const float ServerTimeStamp = ServerData->CurrentClientTimeStamp;
		// This is more severe if the timestamp has a large discrepancy and hasn't been recently reset.
		if (ServerTimeStamp > 1.0f && FMath::Abs(ServerTimeStamp - ClientTimeStamp) > VehicleMovementCVars::NetServerMoveTimestampExpiredWarningThreshold)
		{
			UE_LOG(LogNetPlayerMovement, Warning, TEXT("ServerMove: TimeStamp expired: %f, CurrentTimeStamp: %f, Vehicle: %s"), ClientTimeStamp, ServerTimeStamp, *GetNameSafe(VehicleOwner));
		}
		else
		{
			UE_LOG(LogNetPlayerMovement, Log, TEXT("ServerMove: TimeStamp expired: %f, CurrentTimeStamp: %f, Vehicle: %s"), ClientTimeStamp, ServerTimeStamp, *GetNameSafe(VehicleOwner));
		}
		return;
	}

	bool bServerReadyForClient = true;
	APlayerController* PC = Cast<APlayerController>(VehicleOwner->GetController());
	if (PC)
	{
		bServerReadyForClient = PC->NotifyServerReceivedClientData(VehicleOwner, ClientTimeStamp);
		if (!bServerReadyForClient)
		{
			ClientAccel = FVector::ZeroVector;
		}
	}

	const UWorld* MyWorld = GetWorld();
	const float DeltaTime = ServerData->GetServerMoveDeltaTime(ClientTimeStamp, VehicleOwner->GetActorTimeDilation(*MyWorld));

	if (DeltaTime > 0.f)
	{
		ServerData->CurrentClientTimeStamp = ClientTimeStamp;
		ServerData->ServerAccumulatedClientTimeStamp += DeltaTime;
		ServerData->ServerTimeStamp = MyWorld->GetTimeSeconds();
		ServerData->ServerTimeStampLastServerMove = ServerData->ServerTimeStamp;

		if (AController* VehicleController = Cast<AController>(VehicleOwner->GetController()))
		{
			VehicleController->SetControlRotation(ClientControlRotation);
		}

		if (!bServerReadyForClient)
		{
			return;
		}

		// Perform actual movement
		if ((MyWorld->GetWorldSettings()->GetPauserPlayerState() == NULL))
		{
			if (PC)
			{
				PC->UpdateRotation(DeltaTime);
			}

			MoveAutonomous(ClientTimeStamp, DeltaTime, ClientMoveFlags, ClientAccel);
		}

		/*UE_CLOG(VehicleOwner && UpdatedComponent, LogNetPlayerMovement, VeryVerbose, TEXT("ServerMove Time %f Acceleration %s Velocity %s Position %s Rotation %s DeltaTime %f Mode %s MovementBase %s.%s (Dynamic:%d)"),
			ClientTimeStamp, *ClientAccel.ToString(), *Velocity.ToString(), *UpdatedComponent->GetComponentLocation().ToString(), *UpdatedComponent->GetComponentRotation().ToCompactString(), DeltaTime, *GetMovementName(),
			*GetNameSafe(GetMovementBase()), *VehicleOwner->GetBasedMovement().BoneName.ToString(), MovementBaseUtility::IsDynamicBase(GetMovementBase()) ? 1 : 0);*/
	}

	// Validate move only after old and first dual portion, after all moves are completed.
	if (MoveData.NetworkMoveType == FVehicleNetworkMoveData::ENetworkMoveType::NewMove)
	{
		// ServerMoveHandleClientError(ClientTimeStamp, DeltaTime, ClientAccel, MoveData.Location, MoveData.MovementBase, MoveData.MovementBaseBoneName, MoveData.MovementMode);
	}
}

void UAircraftMovementComponent::MoveAutonomous
(
	float ClientTimeStamp,
	float DeltaTime,
	uint8 CompressedFlags,
	const FVector& NewAccel
)
{
	if (!HasValidData())
	{
		return;
	}

	// UpdateFromCompressedFlags(CompressedFlags);
	// VehicleOwner->CheckJumpInput(DeltaTime);

	Acceleration = ConstrainInputAcceleration(NewAccel);
	Acceleration = Acceleration.GetClampedToMaxSize(GetMaxAcceleration());
	// AnalogInputModifier = ComputeAnalogInputModifier();

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FQuat OldRotation = UpdatedComponent->GetComponentQuat();

	// const bool bWasPlayingRootMotion = VehicleOwner->IsPlayingRootMotion();

	PerformMovement(DeltaTime);

	// Check if data is valid as PerformMovement can mark Vehicle for pending kill
	if (!HasValidData())
	{
		return;
	}

	// If not playing root motion, tick animations after physics. We do this here to keep events, notifies, states and transitions in sync with client updates.
	//if (VehicleOwner && !VehicleOwner->bClientUpdating && !VehicleOwner->IsPlayingRootMotion() && VehicleOwner->GetMesh())
	//{
	//	if (!bWasPlayingRootMotion) // If we were playing root motion before PerformMovement but aren't anymore, we're on the last frame of anim root motion and have already ticked Vehicle
	//	{
	//		TickVehiclePose(DeltaTime);
	//	}
	//	// TODO: SaveBaseLocation() in case tick moves us?

	//	if (!VehicleMovementCVars::EnableQueuedAnimEventsOnServer || VehicleOwner->GetMesh()->ShouldOnlyTickMontages(DeltaTime))
	//	{
	//		// If we're not doing a full anim graph update on the server, 
	//		// trigger events right away, as we could be receiving multiple ServerMoves per frame.
	//		VehicleOwner->GetMesh()->ConditionallyDispatchQueuedAnimEvents();
	//	}
	//}

	if (VehicleOwner && UpdatedComponent)
	{
		// Smooth local view of remote clients on listen servers
		if (VehicleMovementCVars::NetEnableListenServerSmoothing &&
			VehicleOwner->GetRemoteRole() == ROLE_AutonomousProxy &&
			IsNetMode(NM_ListenServer))
		{
			SmoothCorrection(OldLocation, OldRotation, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat());
		}
	}
}

float UAircraftMovementComponent::GetSimulationTimeStep(float RemainingTime, int32 Iterations) const
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
				UE_LOG(LogVehicleMovement, Warning, TEXT("GetSimulationTimeStep() - Max iterations %d hit while remaining time %.6f > MaxSimulationTimeStep (%.3f) for '%s', movement '%s'"), MaxSimulationIterations, RemainingTime, MaxSimulationTimeStep, *GetNameSafe(VehicleOwner), *GetMovementName());
			}
#endif
		}
	}

	// no less than MIN_TICK_TIME (to avoid potential divide-by-zero during simulation).
	return FMath::Max(MIN_TICK_TIME, RemainingTime);
}

void UAircraftMovementComponent::SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation, const FVector& NewLocation, const FQuat& NewRotation)
{
	// SCOPE_CYCLE_COUNTER(STAT_VehicleMovementSmoothCorrection);
	if (!HasValidData())
	{
		return;
	}

	// We shouldn't be running this on a server that is not a listen server.
	checkSlow(GetNetMode() != NM_DedicatedServer);
	checkSlow(GetNetMode() != NM_Standalone);

	// Only client proxies or remote clients on a listen server should run this code.
	const bool bIsSimulatedProxy = (VehicleOwner->GetLocalRole() == ROLE_SimulatedProxy);
	const bool bIsRemoteAutoProxy = (VehicleOwner->GetRemoteRole() == ROLE_AutonomousProxy);
	ensure(bIsSimulatedProxy || bIsRemoteAutoProxy);

	// Getting a correction means new data, so smoothing needs to run.
	// bNetworkSmoothingComplete = false;

	// Handle selected smoothing mode.
	// if (NetworkSmoothingMode == ENetworkSmoothingMode::Disabled)
	{
		UpdatedComponent->SetWorldLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
		// bNetworkSmoothingComplete = true;
	}
	if (FNetworkPredictionData_Client_Vehicle* ClientData = GetPredictionData_Client_Vehicle())
	{
		const UWorld* MyWorld = GetWorld();
		if (!ensure(MyWorld != nullptr))
		{
			return;
		}

		// The mesh doesn't move, but the capsule does so we have a new offset.
		FVector NewToOldVector = (OldLocation - NewLocation);
		// if (bIsNavWalkingOnServer && FMath::Abs(NewToOldVector.Z) < NavWalkingFloorDistTolerance)
		{
			// ignore smoothing on Z axis
			// don't modify new location (local simulation result), since it's probably more accurate than server data
			// and shouldn't matter as long as difference is relatively small
			// NewToOldVector.Z = 0;
		}

		const float DistSq = NewToOldVector.SizeSquared();
		if (DistSq > FMath::Square(ClientData->MaxSmoothNetUpdateDist))
		{
			ClientData->MeshTranslationOffset = (DistSq > FMath::Square(ClientData->NoSmoothNetUpdateDist))
				? FVector::ZeroVector
				: ClientData->MeshTranslationOffset + ClientData->MaxSmoothNetUpdateDist * NewToOldVector.GetSafeNormal();
		}
		else
		{
			ClientData->MeshTranslationOffset = ClientData->MeshTranslationOffset + NewToOldVector;
		}

		// UE_LOG(LogVehicleNetSmoothing, Verbose, TEXT("Proxy %s SmoothCorrection(%.2f)"), *GetNameSafe(VehicleOwner), FMath::Sqrt(DistSq));
		// if (NetworkSmoothingMode == ENetworkSmoothingMode::Linear)
		{
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;

			// Remember the current and target rotation, we're going to lerp between them
			ClientData->OriginalMeshRotationOffset = OldRotation;
			ClientData->MeshRotationOffset = OldRotation;
			ClientData->MeshRotationTarget = NewRotation;

			// Move the capsule, but not the mesh.
			// Note: we don't change rotation, we lerp towards it in SmoothClientPosition.
			if (NewLocation != OldLocation)
			{
				// const FScopedPreventAttachedComponentMove PreventMeshMove(VehicleOwner->GetMesh());
				// UpdatedComponent->SetWorldLocation(NewLocation, false, nullptr, GetTeleportType());
			}
		}
		// else
		{
			// Calc rotation needed to keep current world rotation after UpdatedComponent moves.
			// Take difference between where we were rotated before, and where we're going
			ClientData->MeshRotationOffset = (NewRotation.Inverse() * OldRotation) * ClientData->MeshRotationOffset;
			ClientData->MeshRotationTarget = FQuat::Identity;

			// const FScopedPreventAttachedComponentMove PreventMeshMove(VehicleOwner->GetMesh());
			// UpdatedComponent->SetWorldLocationAndRotation(NewLocation, NewRotation, false, nullptr, GetTeleportType());
		}

		//////////////////////////////////////////////////////////////////////////
		// Update smoothing timestamps

		// If running ahead, pull back slightly. This will cause the next delta to seem slightly longer, and cause us to lerp to it slightly slower.
		if (ClientData->SmoothingClientTimeStamp > ClientData->SmoothingServerTimeStamp)
		{
			const double OldClientTimeStamp = ClientData->SmoothingClientTimeStamp;
			ClientData->SmoothingClientTimeStamp = FMath::LerpStable(ClientData->SmoothingServerTimeStamp, OldClientTimeStamp, 0.5);

			UE_LOG(LogVehicleNetSmoothing, VeryVerbose, TEXT("SmoothCorrection: Pull back client from ClientTimeStamp: %.6f to %.6f, ServerTimeStamp: %.6f for %s"),
				OldClientTimeStamp, ClientData->SmoothingClientTimeStamp, ClientData->SmoothingServerTimeStamp, *GetNameSafe(VehicleOwner));
		}

		// Using server timestamp lets us know how much time actually elapsed, regardless of packet lag variance.
		double OldServerTimeStamp = ClientData->SmoothingServerTimeStamp;
		if (bIsSimulatedProxy)
		{
			// This value is normally only updated on the server, however some code paths might try to read it instead of the replicated value so copy it for proxies as well.
			// ServerLastTransformUpdateTimeStamp = VehicleOwner->GetReplicatedServerLastTransformUpdateTimeStamp();
		}
		// ClientData->SmoothingServerTimeStamp = ServerLastTransformUpdateTimeStamp;

		// Initial update has no delta.
		if (ClientData->LastCorrectionTime == 0)
		{
			ClientData->SmoothingClientTimeStamp = ClientData->SmoothingServerTimeStamp;
			OldServerTimeStamp = ClientData->SmoothingServerTimeStamp;
		}

		// Don't let the client fall too far behind or run ahead of new server time.
		const double ServerDeltaTime = ClientData->SmoothingServerTimeStamp - OldServerTimeStamp;
		const double MaxOffset = ClientData->MaxClientSmoothingDeltaTime;
		const double MinOffset = FMath::Min(double(ClientData->SmoothNetUpdateTime), MaxOffset);

		// MaxDelta is the farthest behind we're allowed to be after receiving a new server time.
		const double MaxDelta = FMath::Clamp(ServerDeltaTime * 1.25, MinOffset, MaxOffset);
		ClientData->SmoothingClientTimeStamp = FMath::Clamp(ClientData->SmoothingClientTimeStamp, ClientData->SmoothingServerTimeStamp - MaxDelta, ClientData->SmoothingServerTimeStamp);

		// Compute actual delta between new server timestamp and client simulation.
		ClientData->LastCorrectionDelta = ClientData->SmoothingServerTimeStamp - ClientData->SmoothingClientTimeStamp;
		ClientData->LastCorrectionTime = MyWorld->GetTimeSeconds();

		/*UE_LOG(LogVehicleNetSmoothing, VeryVerbose, TEXT("SmoothCorrection: WorldTime: %.6f, ServerTimeStamp: %.6f, ClientTimeStamp: %.6f, Delta: %.6f for %s"),
			MyWorld->GetTimeSeconds(), ClientData->SmoothingServerTimeStamp, ClientData->SmoothingClientTimeStamp, ClientData->LastCorrectionDelta, *GetNameSafe(VehicleOwner));*/

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		if (VehicleMovementCVars::NetVisualizeSimulatedCorrections >= 2)
		{
			const float Radius = 4.0f;
			const bool	bPersist = false;
			const float Lifetime = 10.0f;
			const int32	Sides = 8;
			const float ArrowSize = 4.0f;

			const FVector SimulatedLocation = OldLocation;
			const FVector ServerLocation = NewLocation + FVector(0, 0, 0.5f);

			// const FVector SmoothLocation = VehicleOwner->GetMesh()->GetComponentLocation() - VehicleOwner->GetBaseTranslationOffset() + FVector(0, 0, 1.0f);

			//DrawDebugCoordinateSystem( GetWorld(), ServerLocation + FVector( 0, 0, 300.0f ), UpdatedComponent->GetComponentRotation(), 45.0f, bPersist, Lifetime );

			// Draw simulated location
			DrawCircle(GetWorld(), SimulatedLocation, FVector(1, 0, 0), FVector(0, 1, 0), FColor(255, 0, 0, 255), Radius, Sides, bPersist, Lifetime);

			// Draw server (corrected location)
			DrawCircle(GetWorld(), ServerLocation, FVector(1, 0, 0), FVector(0, 1, 0), FColor(0, 255, 0, 255), Radius, Sides, bPersist, Lifetime);

			// Draw smooth simulated location
			// FRotationMatrix SmoothMatrix(VehicleOwner->GetMesh()->GetComponentRotation());
			// DrawDebugDirectionalArrow(GetWorld(), SmoothLocation, SmoothLocation + SmoothMatrix.GetScaledAxis(EAxis::Y) * 5, ArrowSize, FColor(255, 255, 0, 255), bPersist, Lifetime);
			// DrawCircle(GetWorld(), SmoothLocation, FVector(1, 0, 0), FVector(0, 1, 0), FColor(0, 0, 255, 255), Radius, Sides, bPersist, Lifetime);

			if (ClientData->LastServerLocation != FVector::ZeroVector)
			{
				// Arrow showing simulated line
				DrawDebugDirectionalArrow(GetWorld(), ClientData->LastServerLocation, SimulatedLocation, ArrowSize, FColor(255, 0, 0, 255), bPersist, Lifetime);

				// Arrow showing server line
				DrawDebugDirectionalArrow(GetWorld(), ClientData->LastServerLocation, ServerLocation, ArrowSize, FColor(0, 255, 0, 255), bPersist, Lifetime);

				// Arrow showing smooth location plot
				// DrawDebugDirectionalArrow(GetWorld(), ClientData->LastSmoothLocation, SmoothLocation, ArrowSize, FColor(0, 0, 255, 255), bPersist, Lifetime);

				// Line showing correction
				DrawDebugDirectionalArrow(GetWorld(), SimulatedLocation, ServerLocation, ArrowSize, FColor(128, 0, 0, 255), bPersist, Lifetime);

				// Line showing smooth vector
				// DrawDebugDirectionalArrow(GetWorld(), ServerLocation, SmoothLocation, ArrowSize, FColor(0, 0, 128, 255), bPersist, Lifetime);
			}

			ClientData->LastServerLocation = ServerLocation;
			// ClientData->LastSmoothLocation = SmoothLocation;
		}
#endif
	}
}

bool UAircraftMovementComponent::VerifyClientTimeStamp(float TimeStamp, FNetworkPredictionData_Server_Vehicle& ServerData)
{
	UWorld* World = GetWorld();

	const bool bFirstMoveAfterForcedUpdates = ServerData.bTriggeringForcedUpdates;
	if (bFirstMoveAfterForcedUpdates)
	{
		// We have been performing ForcedUpdates because we hadn't received any moves from this connection in a while but we've now received a new move!
		// Let's sync up to this TimeStamp in order to resolve movement desyncs ASAP
		// This will result in this move having a zero DeltaTime, so it will perform no movement but it will send a correction, and we should be able to process the next move that arrives.
		UE_LOG(LogNetPlayerMovement, Log, TEXT("Received a new move after performing ForcedUpdates.  Updating CurrentClientTimeStamp from %f to %f"), ServerData.CurrentClientTimeStamp, TimeStamp);
		ServerData.CurrentClientTimeStamp = TimeStamp;
		if (World != nullptr)
		{
			ServerData.ServerTimeStamp = World->GetTimeSeconds();
		}
	}

	bool bTimeStampResetDetected = false;
	bool bNeedsForcedUpdate = false;
	const bool bIsValid = bFirstMoveAfterForcedUpdates || IsClientTimeStampValid(TimeStamp, ServerData, bTimeStampResetDetected);
	if (bIsValid)
	{
		if (bTimeStampResetDetected)
		{
			UE_LOG(LogNetPlayerMovement, Log, TEXT("TimeStamp reset detected. CurrentTimeStamp: %f, new TimeStamp: %f"), ServerData.CurrentClientTimeStamp, TimeStamp);
			if (World != nullptr)
			{
				LastTimeStampResetServerTime = World->GetTimeSeconds();
			}
			OnClientTimeStampResetDetected();
			ServerData.CurrentClientTimeStamp -= MinTimeBetweenTimeStampResets;

			// Also apply the reset to any active root motions.
			// CurrentRootMotion.ApplyTimeStampReset(MinTimeBetweenTimeStampResets);
		}
		else
		{
			UE_LOG(LogNetPlayerMovement, VeryVerbose, TEXT("TimeStamp %f Accepted! CurrentTimeStamp: %f"), TimeStamp, ServerData.CurrentClientTimeStamp);
			ProcessClientTimeStampForTimeDiscrepancy(TimeStamp, ServerData);
		}
	}
	else
	{
		if (bTimeStampResetDetected)
		{
			UE_LOG(LogNetPlayerMovement, Log, TEXT("TimeStamp expired. Before TimeStamp Reset. CurrentTimeStamp: %f, TimeStamp: %f"), ServerData.CurrentClientTimeStamp, TimeStamp);
		}
		else
		{
			bNeedsForcedUpdate = (TimeStamp <= ServerData.LastReceivedClientTimeStamp);
		}
	}

	ServerData.LastReceivedClientTimeStamp = TimeStamp;
	ServerData.bLastRequestNeedsForcedUpdates = bNeedsForcedUpdate;
	return bIsValid;
}

void UAircraftMovementComponent::ProcessClientTimeStampForTimeDiscrepancy(float ClientTimeStamp, FNetworkPredictionData_Server_Vehicle& ServerData)
{
	// Should only be called on server in network games
	check(VehicleOwner != NULL);
	check(VehicleOwner->GetLocalRole() == ROLE_Authority);
	checkSlow(GetNetMode() < NM_Client);

	// Movement time discrepancy detection and resolution (potentially caused by client speed hacks, time manipulation)
	// Track client reported time deltas through ServerMove RPCs vs actual server time, when error accumulates enough
	// trigger prevention measures where client must "pay back" the time difference
	const bool bServerMoveHasOccurred = ServerData.ServerTimeStampLastServerMove != 0.f;
	const AGameNetworkManager* GameNetworkManager = (const AGameNetworkManager*)(AGameNetworkManager::StaticClass()->GetDefaultObject());
	if (GameNetworkManager != nullptr && GameNetworkManager->bMovementTimeDiscrepancyDetection && bServerMoveHasOccurred)
	{
		const float WorldTimeSeconds = GetWorld()->GetTimeSeconds();
		const float ServerDelta = (WorldTimeSeconds - ServerData.ServerTimeStamp) * VehicleOwner->CustomTimeDilation;
		const float ClientDelta = ClientTimeStamp - ServerData.CurrentClientTimeStamp;
		const float ClientError = ClientDelta - ServerDelta; // Difference between how much time client has ticked since last move vs server

		// Accumulate raw total discrepancy, unfiltered/unbound (for tracking more long-term trends over the lifetime of the VehicleMovementComponent)
		ServerData.LifetimeRawTimeDiscrepancy += ClientError;

		//
		// 1. Determine total effective discrepancy 
		//
		// NewTimeDiscrepancy is bounded and has a DriftAllowance to limit momentary burst packet loss or 
		// low framerate from having significant impacts, which could cause needing multiple seconds worth of 
		// slow-down/speed-up even though it wasn't intentional time manipulation
		float NewTimeDiscrepancy = ServerData.TimeDiscrepancy + ClientError;
		{
			// Apply drift allowance - forgiving percent difference per time for error
			const float DriftAllowance = GameNetworkManager->MovementTimeDiscrepancyDriftAllowance;
			if (DriftAllowance > 0.f)
			{
				if (NewTimeDiscrepancy > 0.f)
				{
					NewTimeDiscrepancy = FMath::Max(NewTimeDiscrepancy - ServerDelta * DriftAllowance, 0.f);
				}
				else
				{
					NewTimeDiscrepancy = FMath::Min(NewTimeDiscrepancy + ServerDelta * DriftAllowance, 0.f);
				}
			}

			// Enforce bounds
			// Never go below MinTimeMargin - ClientError being negative means the client is BEHIND
			// the server (they are going slower).
			NewTimeDiscrepancy = FMath::Max(NewTimeDiscrepancy, GameNetworkManager->MovementTimeDiscrepancyMinTimeMargin);
		}

		// Determine EffectiveClientError, which is error for the currently-being-processed move after 
		// drift allowances/clamping/resolution mode modifications.
		// We need to know how much the current move contributed towards actionable error so that we don't
		// count error that the server never allowed to impact movement to matter
		float EffectiveClientError = ClientError;
		{
			const float NewTimeDiscrepancyRaw = ServerData.TimeDiscrepancy + ClientError;
			if (NewTimeDiscrepancyRaw != 0.f)
			{
				EffectiveClientError = ClientError * (NewTimeDiscrepancy / NewTimeDiscrepancyRaw);
			}
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		// Per-frame spew of time discrepancy-related values - useful for investigating state of time discrepancy tracking
		if (VehicleMovementCVars::DebugTimeDiscrepancy > 0)
		{
			UE_LOG(LogNetPlayerMovement, Warning, TEXT("TimeDiscrepancyDetection: ClientError: %f, TimeDiscrepancy: %f, LifetimeRawTimeDiscrepancy: %f (Lifetime %f), Resolving: %d, ClientDelta: %f, ServerDelta: %f, ClientTimeStamp: %f"),
				ClientError, ServerData.TimeDiscrepancy, ServerData.LifetimeRawTimeDiscrepancy, WorldTimeSeconds - ServerData.WorldCreationTime, ServerData.bResolvingTimeDiscrepancy, ClientDelta, ServerDelta, ClientTimeStamp);
		}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

		//
		// 2. If we were in resolution mode, determine if we still need to be
		//
		ServerData.bResolvingTimeDiscrepancy = ServerData.bResolvingTimeDiscrepancy && (ServerData.TimeDiscrepancy > 0.f);

		//
		// 3. Determine if NewTimeDiscrepancy is significant enough to trigger detection, and if so, trigger resolution if enabled
		//
		if (!ServerData.bResolvingTimeDiscrepancy)
		{
			if (NewTimeDiscrepancy > GameNetworkManager->MovementTimeDiscrepancyMaxTimeMargin)
			{
				// Time discrepancy detected - client timestamp ahead of where the server thinks it should be!

				// Trigger logic for resolving time discrepancies
				if (GameNetworkManager->bMovementTimeDiscrepancyResolution)
				{
					// Trigger Resolution
					ServerData.bResolvingTimeDiscrepancy = true;

					// Transfer calculated error to official TimeDiscrepancy value, which is the time that will be resolved down
					// in this and subsequent moves until it reaches 0 (meaning we equalize the error)
					// Don't include contribution to error for this move, since we are now going to be in resolution mode
					// and the expected client error (though it did help trigger resolution) won't be allowed
					// to increase error this frame
					ServerData.TimeDiscrepancy = (NewTimeDiscrepancy - EffectiveClientError);
				}
				else
				{
					// We're detecting discrepancy but not handling resolving that through movement component.
					// Clear time stamp error accumulated that triggered detection so we start fresh (maybe it was triggered
					// during severe hitches/packet loss/other non-goodness)
					ServerData.TimeDiscrepancy = 0.f;
				}

				// Project-specific resolution (reporting/recording/analytics)
				OnTimeDiscrepancyDetected(NewTimeDiscrepancy, ServerData.LifetimeRawTimeDiscrepancy, WorldTimeSeconds - ServerData.WorldCreationTime, ClientError);
			}
			else
			{
				// When not in resolution mode and still within error tolerances, accrue total discrepancy
				ServerData.TimeDiscrepancy = NewTimeDiscrepancy;
			}
		}

		//
		// 4. If we are actively resolving time discrepancy, we do so by altering the DeltaTime for the current ServerMove
		//
		if (ServerData.bResolvingTimeDiscrepancy)
		{
			// Optionally force client corrections during time discrepancy resolution
			// This is useful when default project movement error checking is lenient or ClientAuthorativePosition is enabled
			// to ensure time discrepancy resolution is enforced
			if (GameNetworkManager->bMovementTimeDiscrepancyForceCorrectionsDuringResolution)
			{
				ServerData.bForceClientUpdate = true;
			}

			// Movement time discrepancy resolution
			// When the server has detected a significant time difference between what the client ServerMove RPCs are reporting
			// and the actual time that has passed on the server (pointing to potential speed hacks/time manipulation by client),
			// we enter a resolution mode where the usual "base delta's off of client's reported timestamps" is clamped down
			// to the server delta since last movement update, so that during resolution we're not allowing further advantage.
			// Out of that ServerDelta-based move delta, we also need the client to "pay back" the time stolen from initial 
			// time discrepancy detection (held in TimeDiscrepancy) at a specified rate (AGameNetworkManager::TimeDiscrepancyResolutionRate) 
			// to equalize movement time passed on client and server before we can consider the discrepancy "resolved"
			const float ServerCurrentTimeStamp = WorldTimeSeconds;
			const float ServerDeltaSinceLastMovementUpdate = (ServerCurrentTimeStamp - ServerData.ServerTimeStamp) * VehicleOwner->CustomTimeDilation;
			const bool bIsFirstServerMoveThisServerTick = ServerDeltaSinceLastMovementUpdate > 0.f;

			// Restrict ServerMoves to server deltas during time discrepancy resolution 
			// (basing moves off of trusted server time, not client timestamp deltas)
			const float BaseDeltaTime = ServerData.GetBaseServerMoveDeltaTime(ClientTimeStamp, VehicleOwner->GetActorTimeDilation());

			if (!bIsFirstServerMoveThisServerTick)
			{
				// Accumulate client deltas for multiple ServerMoves per server tick so that the next server tick
				// can pay back the full amount of that tick and not be bounded by a single small Move delta
				ServerData.TimeDiscrepancyAccumulatedClientDeltasSinceLastServerTick += BaseDeltaTime;
			}

			float ServerBoundDeltaTime = FMath::Min(BaseDeltaTime + ServerData.TimeDiscrepancyAccumulatedClientDeltasSinceLastServerTick, ServerDeltaSinceLastMovementUpdate);
			ServerBoundDeltaTime = FMath::Max(ServerBoundDeltaTime, 0.f); // No negative deltas allowed

			if (bIsFirstServerMoveThisServerTick)
			{
				// The first ServerMove for a server tick has used the accumulated client delta in the ServerBoundDeltaTime
				// calculation above, clear it out for next frame where we have multiple ServerMoves
				ServerData.TimeDiscrepancyAccumulatedClientDeltasSinceLastServerTick = 0.f;
			}

			// Calculate current move DeltaTime and PayBack time based on resolution rate
			const float ResolutionRate = FMath::Clamp(GameNetworkManager->MovementTimeDiscrepancyResolutionRate, 0.f, 1.f);
			float TimeToPayBack = FMath::Min(ServerBoundDeltaTime * ResolutionRate, ServerData.TimeDiscrepancy); // Make sure we only pay back the time we need to
			float DeltaTimeAfterPayback = ServerBoundDeltaTime - TimeToPayBack;

			// Adjust deltas so current move DeltaTime adheres to minimum tick time
			DeltaTimeAfterPayback = FMath::Max(DeltaTimeAfterPayback, UAircraftMovementComponent::MIN_TICK_TIME);
			TimeToPayBack = ServerBoundDeltaTime - DeltaTimeAfterPayback;

			// Output of resolution: an overridden delta time that will be picked up for this ServerMove, and removing the time
			// we paid back by overriding the DeltaTime to TimeDiscrepancy (time needing resolved)
			ServerData.TimeDiscrepancyResolutionMoveDeltaOverride = DeltaTimeAfterPayback;
			ServerData.TimeDiscrepancy -= TimeToPayBack;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
			// Per-frame spew of time discrepancy resolution related values - useful for investigating state of time discrepancy tracking
			if (VehicleMovementCVars::DebugTimeDiscrepancy > 1)
			{
				UE_LOG(LogNetPlayerMovement, Warning, TEXT("TimeDiscrepancyResolution: DeltaOverride: %f, TimeToPayBack: %f, BaseDelta: %f, ServerDeltaSinceLastMovementUpdate: %f, TimeDiscrepancyAccumulatedClientDeltasSinceLastServerTick: %f"),
					ServerData.TimeDiscrepancyResolutionMoveDeltaOverride, TimeToPayBack, BaseDeltaTime, ServerDeltaSinceLastMovementUpdate, ServerData.TimeDiscrepancyAccumulatedClientDeltasSinceLastServerTick);
			}
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		}
	}
}

bool UAircraftMovementComponent::IsClientTimeStampValid(float TimeStamp, const FNetworkPredictionData_Server_Vehicle& ServerData, bool& bTimeStampResetDetected) const
{
	if (TimeStamp <= 0.f || !FMath::IsFinite(TimeStamp))
	{
		return false;
	}

	// Very large deltas happen around a TimeStamp reset.
	const float DeltaTimeStamp = (TimeStamp - ServerData.CurrentClientTimeStamp);
	if (FMath::Abs(DeltaTimeStamp) > (MinTimeBetweenTimeStampResets * 0.5f))
	{
		// Client is resetting TimeStamp to increase accuracy.
		bTimeStampResetDetected = true;
		if (DeltaTimeStamp < 0.f)
		{
			// Validate that elapsed time since last reset is reasonable, otherwise client could be manipulating resets.
			if (GetWorld()->TimeSince(LastTimeStampResetServerTime) < (MinTimeBetweenTimeStampResets * 0.5f))
			{
				// Reset too recently
				return false;
			}
			else
			{
				// TimeStamp accepted with reset
				return true;
			}
		}
		else
		{
			// We already reset the TimeStamp, but we just got an old outdated move before the switch, not valid.
			return false;
		}
	}

	// If TimeStamp is in the past, move is outdated, not valid.
	if (TimeStamp <= ServerData.CurrentClientTimeStamp)
	{
		return false;
	}

	// Precision issues (or reordered timestamps from old moves) can cause very small or zero deltas which cause problems.
	if (DeltaTimeStamp < UAircraftMovementComponent::MIN_TICK_TIME)
	{
		return false;
	}

	// TimeStamp valid.
	return true;
}

void UAircraftMovementComponent::OnClientTimeStampResetDetected()
{
}

void UAircraftMovementComponent::OnTimeDiscrepancyDetected(float CurrentTimeDiscrepancy, float LifetimeRawTimeDiscrepancy, float Lifetime, float CurrentMoveError)
{
	UE_LOG(LogNetPlayerMovement, Verbose, TEXT("Movement Time Discrepancy detected between client-reported time and server on Vehicle %s. CurrentTimeDiscrepancy: %f, LifetimeRawTimeDiscrepancy: %f, Lifetime: %f, CurrentMoveError %f"),
		VehicleOwner ? *VehicleOwner->GetHumanReadableName() : TEXT("<UNKNOWN>"),
		CurrentTimeDiscrepancy,
		LifetimeRawTimeDiscrepancy,
		Lifetime,
		CurrentMoveError);
}



void UAircraftMovementComponent::PostLoad()
{
	Super::PostLoad();

	VehicleOwner = Cast<AGAVehicle>(PawnOwner);
}

void UAircraftMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	FVector InputVector = FVector::ZeroVector;
	// InputVector = ConsumeInputVector();

	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Super tick may destroy/invalidate VehicleOwner or UpdatedComponent, so we need to re-check.
	if (!HasValidData())
	{
		return;
	}

	if (VehicleOwner->GetLocalRole() > ROLE_SimulatedProxy)
	{
		// Allow root motion to move Vehicles that have no controller.
		if (VehicleOwner->IsLocallyControlled())
		{
			ControlledVehicleMove(InputVector, DeltaTime);
		}
	}
}

bool UAircraftMovementComponent::HasValidData() const
{
	const bool bIsValid = UpdatedComponent && IsValid(VehicleOwner);

	return bIsValid;
}

void UAircraftMovementComponent::ControlledVehicleMove(const FVector& InInputVector, float DeltaTime)
{
	{
		// SCOPE_CYCLE_COUNTER(STAT_CharUpdateAcceleration);

		// We need to check the jump state before adjusting input acceleration, to minimize latency
		// and to make sure acceleration respects our potentially new falling state.
		// VehicleOwner->CheckJumpInput(DeltaTime);

		// apply input to acceleration
		// Acceleration = ScaleInputAcceleration(ConstrainInputAcceleration(InputVector));
		// AnalogInputModifier = ComputeAnalogInputModifier();
	}

	if (VehicleOwner->GetLocalRole() == ROLE_Authority)
	{
		PerformMovement(DeltaTime);
	}
	else if (VehicleOwner->GetLocalRole() == ROLE_AutonomousProxy && IsNetMode(NM_Client))
	{
		ReplicateMoveToServer(DeltaTime, Acceleration);
	}
}

void UAircraftMovementComponent::PerformMovement(float DeltaTime)
{
	// SCOPE_CYCLE_COUNTER(STAT_VehicleMovementPerformMovement);

	const UWorld* MyWorld = GetWorld();
	if (!HasValidData() || MyWorld == nullptr)
	{
		return;
	}

//	bTeleportedSinceLastUpdate = UpdatedComponent->GetComponentLocation() != LastUpdateLocation;
//
	// no movement if we can't move, or if currently doing physical simulation on UpdatedComponent
	if (MovementMode == MOVE_None || UpdatedComponent->Mobility != EComponentMobility::Movable || UpdatedComponent->IsSimulatingPhysics())
	{
		//if (!VehicleOwner->bClientUpdating && !VehicleOwner->bServerMoveIgnoreRootMotion)
		//{
		//	// Consume root motion
		//	if (VehicleOwner->IsPlayingRootMotion() && VehicleOwner->GetMesh())
		//	{
		//		TickVehiclePose(DeltaTime);
		//		RootMotionParams.Clear();
		//	}
		//	if (CurrentRootMotion.HasActiveRootMotionSources())
		//	{
		//		CurrentRootMotion.Clear();
		//	}
		//}
		// Clear pending physics forces
		ClearAccumulatedForces();
		return;
	}

//	// Force floor update if we've moved outside of VehicleMovement since last update.
//	bForceNextFloorCheck |= (IsMovingOnGround() && bTeleportedSinceLastUpdate);
//
//	// Update saved LastPreAdditiveVelocity with any external changes to Vehicle Velocity that happened since last update.
//	if (CurrentRootMotion.HasAdditiveVelocity())
//	{
//		const FVector Adjustment = (Velocity - LastUpdateVelocity);
//		CurrentRootMotion.LastPreAdditiveVelocity += Adjustment;
//
//#if ROOT_MOTION_DEBUG
//		if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
//		{
//			if (!Adjustment.IsNearlyZero())
//			{
//				FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement HasAdditiveVelocity LastUpdateVelocityAdjustment LastPreAdditiveVelocity(%s) Adjustment(%s)"),
//					*CurrentRootMotion.LastPreAdditiveVelocity.ToCompactString(), *Adjustment.ToCompactString());
//				RootMotionSourceDebug::PrintOnScreen(*VehicleOwner, AdjustedDebugString);
//			}
//		}
//#endif
//	}

	FVector OldVelocity;
	FVector OldLocation;

	// Scoped updates can improve performance of multiple MoveComponent calls.
	{
//		FScopedMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates ? EScopedUpdate::DeferredUpdates : EScopedUpdate::ImmediateUpdates);
//
//		MaybeUpdateBasedMovement(DeltaTime);
//
//		// Clean up invalid RootMotion Sources.
//		// This includes RootMotion sources that ended naturally.
//		// They might want to perform a clamp on velocity or an override, 
//		// so we want this to happen before ApplyAccumulatedForces and HandlePendingLaunch as to not clobber these.
//		const bool bHasRootMotionSources = HasRootMotionSources();
//		if (bHasRootMotionSources && !VehicleOwner->bClientUpdating && !VehicleOwner->bServerMoveIgnoreRootMotion)
//		{
//			SCOPE_CYCLE_COUNTER(STAT_VehicleMovementRootMotionSourceCalculate);
//
//			const FVector VelocityBeforeCleanup = Velocity;
//			CurrentRootMotion.CleanUpInvalidRootMotion(DeltaTime, *VehicleOwner, *this);
//
//#if ROOT_MOTION_DEBUG
//			if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
//			{
//				if (Velocity != VelocityBeforeCleanup)
//				{
//					const FVector Adjustment = Velocity - VelocityBeforeCleanup;
//					FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement CleanUpInvalidRootMotion Velocity(%s) VelocityBeforeCleanup(%s) Adjustment(%s)"),
//						*Velocity.ToCompactString(), *VelocityBeforeCleanup.ToCompactString(), *Adjustment.ToCompactString());
//					RootMotionSourceDebug::PrintOnScreen(*VehicleOwner, AdjustedDebugString);
//				}
//			}
//#endif
//		}

		OldVelocity = Velocity;
		OldLocation = UpdatedComponent->GetComponentLocation();

		ApplyAccumulatedForces(DeltaTime);

		// Update the Vehicle state before we do our movement
		UpdateVehicleStateBeforeMovement(DeltaTime);

		/*if (MovementMode == MOVE_NavWalking && bWantsToLeaveNavWalking)
		{
			TryToLeaveNavWalking();
		}*/

		// Vehicle::LaunchVehicle() has been deferred until now.
		HandlePendingLaunch();
		ClearAccumulatedForces();

//#if ROOT_MOTION_DEBUG
//		if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
//		{
//			if (OldVelocity != Velocity)
//			{
//				const FVector Adjustment = Velocity - OldVelocity;
//				FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement ApplyAccumulatedForces+HandlePendingLaunch Velocity(%s) OldVelocity(%s) Adjustment(%s)"),
//					*Velocity.ToCompactString(), *OldVelocity.ToCompactString(), *Adjustment.ToCompactString());
//				RootMotionSourceDebug::PrintOnScreen(*VehicleOwner, AdjustedDebugString);
//			}
//		}
//#endif

//		// Update saved LastPreAdditiveVelocity with any external changes to Vehicle Velocity that happened due to ApplyAccumulatedForces/HandlePendingLaunch
//		if (CurrentRootMotion.HasAdditiveVelocity())
//		{
//			const FVector Adjustment = (Velocity - OldVelocity);
//			CurrentRootMotion.LastPreAdditiveVelocity += Adjustment;
//
//#if ROOT_MOTION_DEBUG
//			if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
//			{
//				if (!Adjustment.IsNearlyZero())
//				{
//					FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement HasAdditiveVelocity AccumulatedForces LastPreAdditiveVelocity(%s) Adjustment(%s)"),
//						*CurrentRootMotion.LastPreAdditiveVelocity.ToCompactString(), *Adjustment.ToCompactString());
//					RootMotionSourceDebug::PrintOnScreen(*VehicleOwner, AdjustedDebugString);
//				}
//			}
//#endif
//		}

//		// Prepare Root Motion (generate/accumulate from root motion sources to be used later)
//		if (bHasRootMotionSources && !VehicleOwner->bClientUpdating && !VehicleOwner->bServerMoveIgnoreRootMotion)
//		{
//			// Animation root motion - If using animation RootMotion, tick animations before running physics.
//			if (VehicleOwner->IsPlayingRootMotion() && VehicleOwner->GetMesh())
//			{
//				TickVehiclePose(DeltaTime);
//
//				// Make sure animation didn't trigger an event that destroyed us
//				if (!HasValidData())
//				{
//					return;
//				}
//
//				// For local human clients, save off root motion data so it can be used by movement networking code.
//				if (VehicleOwner->IsLocallyControlled() && (VehicleOwner->GetLocalRole() == ROLE_AutonomousProxy) && VehicleOwner->IsPlayingNetworkedRootMotionMontage())
//				{
//					VehicleOwner->ClientRootMotionParams = RootMotionParams;
//				}
//			}
//
//			// Generates root motion to be used this frame from sources other than animation
//			{
//				SCOPE_CYCLE_COUNTER(STAT_VehicleMovementRootMotionSourceCalculate);
//				CurrentRootMotion.PrepareRootMotion(DeltaTime, *VehicleOwner, *this, true);
//			}
//
//			// For local human clients, save off root motion data so it can be used by movement networking code.
//			if (VehicleOwner->IsLocallyControlled() && (VehicleOwner->GetLocalRole() == ROLE_AutonomousProxy))
//			{
//				VehicleOwner->SavedRootMotion = CurrentRootMotion;
//			}
//		}

//		// Apply Root Motion to Velocity
//		if (CurrentRootMotion.HasOverrideVelocity() || HasAnimRootMotion())
//		{
//			// Animation root motion overrides Velocity and currently doesn't allow any other root motion sources
//			if (HasAnimRootMotion())
//			{
//				// Convert to world space (animation root motion is always local)
//				USkeletalMeshComponent* SkelMeshComp = VehicleOwner->GetMesh();
//				if (SkelMeshComp)
//				{
//					// Convert Local Space Root Motion to world space. Do it right before used by physics to make sure we use up to date transforms, as translation is relative to rotation.
//					RootMotionParams.Set(ConvertLocalRootMotionToWorld(RootMotionParams.GetRootMotionTransform(), DeltaTime));
//				}
//
//				// Then turn root motion to velocity to be used by various physics modes.
//				if (DeltaTime > 0.f)
//				{
//					AnimRootMotionVelocity = CalcAnimRootMotionVelocity(RootMotionParams.GetRootMotionTransform().GetTranslation(), DeltaTime, Velocity);
//					Velocity = ConstrainAnimRootMotionVelocity(AnimRootMotionVelocity, Velocity);
//					if (IsFalling())
//					{
//						Velocity += FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f);
//					}
//				}
//
//				UE_LOG(LogRootMotion, Log, TEXT("PerformMovement WorldSpaceRootMotion Translation: %s, Rotation: %s, Actor Facing: %s, Velocity: %s")
//					, *RootMotionParams.GetRootMotionTransform().GetTranslation().ToCompactString()
//					, *RootMotionParams.GetRootMotionTransform().GetRotation().Rotator().ToCompactString()
//					, *VehicleOwner->GetActorForwardVector().ToCompactString()
//					, *Velocity.ToCompactString()
//				);
//			}
//			else
//			{
//				// We don't have animation root motion so we apply other sources
//				if (DeltaTime > 0.f)
//				{
//					SCOPE_CYCLE_COUNTER(STAT_VehicleMovementRootMotionSourceApply);
//
//					const FVector VelocityBeforeOverride = Velocity;
//					FVector NewVelocity = Velocity;
//					CurrentRootMotion.AccumulateOverrideRootMotionVelocity(DeltaTime, *VehicleOwner, *this, NewVelocity);
//					if (IsFalling())
//					{
//						NewVelocity += CurrentRootMotion.HasOverrideVelocityWithIgnoreZAccumulate() ? FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f) : DecayingFormerBaseVelocity;
//					}
//					Velocity = NewVelocity;
//
//#if ROOT_MOTION_DEBUG
//					if (RootMotionSourceDebug::CVarDebugRootMotionSources.GetValueOnGameThread() == 1)
//					{
//						if (VelocityBeforeOverride != Velocity)
//						{
//							FString AdjustedDebugString = FString::Printf(TEXT("PerformMovement AccumulateOverrideRootMotionVelocity Velocity(%s) VelocityBeforeOverride(%s)"),
//								*Velocity.ToCompactString(), *VelocityBeforeOverride.ToCompactString());
//							RootMotionSourceDebug::PrintOnScreen(*VehicleOwner, AdjustedDebugString);
//						}
//					}
//#endif
//				}
//			}
//		}

		// Clear jump input now, to allow movement events to trigger it for next update.
		// VehicleOwner->ClearJumpInput(DeltaTime);
		// NumJumpApexAttempts = 0;

		// change position
		StartNewPhysics(DeltaTime, 0);

		if (!HasValidData())
		{
			return;
		}

		// Update Vehicle state based on change from movement
		UpdateVehicleStateAfterMovement(DeltaTime);

		if (true /*bAllowPhysicsRotationDuringAnimRootMotion || !HasAnimRootMotion()*/)
		{
			PhysicsRotation(DeltaTime);
		}

//		// Apply Root Motion rotation after movement is complete.
//		if (HasAnimRootMotion())
//		{
//			const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
//			const FQuat RootMotionRotationQuat = RootMotionParams.GetRootMotionTransform().GetRotation();
//			if (!RootMotionRotationQuat.IsIdentity())
//			{
//				const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
//				MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
//			}
//
//#if !(UE_BUILD_SHIPPING)
//			// debug
//			if (false)
//			{
//				const FRotator OldActorRotation = OldActorRotationQuat.Rotator();
//				const FVector ResultingLocation = UpdatedComponent->GetComponentLocation();
//				const FRotator ResultingRotation = UpdatedComponent->GetComponentRotation();
//
//				// Show current position
//				DrawDebugCoordinateSystem(MyWorld, VehicleOwner->GetMesh()->GetComponentLocation() + FVector(0, 0, 1), ResultingRotation, 50.f, false);
//
//				// Show resulting delta move.
//				DrawDebugLine(MyWorld, OldLocation, ResultingLocation, FColor::Red, false, 10.f);
//
//				// Log details.
//				UE_LOG(LogRootMotion, Warning, TEXT("PerformMovement Resulting DeltaMove Translation: %s, Rotation: %s, MovementBase: %s"), //-V595
//					*(ResultingLocation - OldLocation).ToCompactString(), *(ResultingRotation - OldActorRotation).GetNormalized().ToCompactString(), *GetNameSafe(VehicleOwner->GetMovementBase()));
//
//				const FVector RMTranslation = RootMotionParams.GetRootMotionTransform().GetTranslation();
//				const FRotator RMRotation = RootMotionParams.GetRootMotionTransform().GetRotation().Rotator();
//				UE_LOG(LogRootMotion, Warning, TEXT("PerformMovement Resulting DeltaError Translation: %s, Rotation: %s"),
//					*(ResultingLocation - OldLocation - RMTranslation).ToCompactString(), *(ResultingRotation - OldActorRotation - RMRotation).GetNormalized().ToCompactString());
//			}
//#endif // !(UE_BUILD_SHIPPING)
//
//			// Root Motion has been used, clear
//			RootMotionParams.Clear();
//		}
//		else if (CurrentRootMotion.HasActiveRootMotionSources())
//		{
//			FQuat RootMotionRotationQuat;
//			if (VehicleOwner && UpdatedComponent && CurrentRootMotion.GetOverrideRootMotionRotation(DeltaTime, *VehicleOwner, *this, RootMotionRotationQuat))
//			{
//				const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
//				const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
//				MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
//			}
//		}

		// consume path following requested velocity
		// LastUpdateRequestedVelocity = bHasRequestedVelocity ? RequestedVelocity : FVector::ZeroVector;
		// bHasRequestedVelocity = false;

		OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);
	} // End scoped movement update

//	// Call external post-movement events. These happen after the scoped movement completes in case the events want to use the current state of overlaps etc.
//	CallMovementUpdateDelegate(DeltaTime, OldLocation, OldVelocity);
//
//	if (VehicleMovementCVars::BasedMovementMode == 0)
//	{
//		SaveBaseLocation(); // behaviour before implementing this fix
//	}
//	else
//	{
//		MaybeSaveBaseLocation();
//	}
//	UpdateComponentVelocity();
//
//	const bool bHasAuthority = VehicleOwner && VehicleOwner->HasAuthority();
//
//	// If we move we want to avoid a long delay before replication catches up to notice this change, especially if it's throttling our rate.
//	if (bHasAuthority && UNetDriver::IsAdaptiveNetUpdateFrequencyEnabled() && UpdatedComponent)
//	{
//		UNetDriver* NetDriver = MyWorld->GetNetDriver();
//		if (NetDriver && NetDriver->IsServer())
//		{
//			FNetworkObjectInfo* NetActor = NetDriver->FindNetworkObjectInfo(VehicleOwner);
//
//			if (NetActor && MyWorld->GetTimeTime() <= NetActor->NextUpdateTime && NetDriver->IsNetworkActorUpdateFrequencyThrottled(*NetActor))
//			{
//				if (ShouldCancelAdaptiveReplication())
//				{
//					NetDriver->CancelAdaptiveReplication(*NetActor);
//				}
//			}
//		}
//	}

//	const FVector NewLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
//	const FQuat NewRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;
//
//	if (bHasAuthority && UpdatedComponent && !IsNetMode(NM_Client))
//	{
//		const bool bLocationChanged = (NewLocation != LastUpdateLocation);
//		const bool bRotationChanged = (NewRotation != LastUpdateRotation);
//		if (bLocationChanged || bRotationChanged)
//		{
//			// Update ServerLastTransformUpdateTimeStamp. This is used by Linear smoothing on clients to interpolate positions with the correct delta time,
//			// so the timestamp should be based on the client's move delta (ServerAccumulatedClientTimeStamp), not the server time when receiving the RPC.
//			const bool bIsRemotePlayer = (VehicleOwner->GetRemoteRole() == ROLE_AutonomousProxy);
//			const FNetworkPredictionData_Server_Vehicle* ServerData = bIsRemotePlayer ? GetPredictionData_Server_Vehicle() : nullptr;
//			if (bIsRemotePlayer && ServerData && VehicleMovementCVars::NetUseClientTimestampForReplicatedTransform)
//			{
//				ServerLastTransformUpdateTimeStamp = float(ServerData->ServerAccumulatedClientTimeStamp);
//			}
//			else
//			{
//				ServerLastTransformUpdateTimeStamp = MyWorld->GetTimeTime();
//			}
//		}
//	}
//
//	LastUpdateLocation = NewLocation;
//	LastUpdateRotation = NewRotation;
//	LastUpdateVelocity = Velocity;
}


void UAircraftMovementComponent::UpdateVehicleStateBeforeMovement(float DeltaSeconds)
{
	// Used for detecting changes to crouch state in VehicleMovementComponent
}

void UAircraftMovementComponent::UpdateVehicleStateAfterMovement(float DeltaSeconds)
{
	// Used for detecting changes to crouch state in VehicleMovementComponent
}

void UAircraftMovementComponent::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	// Override for subclasses
}



void UAircraftMovementComponent::ApplyAccumulatedForces(float DeltaTime)
{
	if (PendingImpulseToApply.Z != 0.f || PendingForceToApply.Z != 0.f)
	{
		// check to see if applied momentum is enough to overcome gravity
		if (IsMovingOnGround() && (PendingImpulseToApply.Z + (PendingForceToApply.Z * DeltaTime) + (GetGravityZ() * DeltaTime) > UE_SMALL_NUMBER))
		{
			// SetMovementMode(MOVE_Falling);
		}
	}

	Velocity += PendingImpulseToApply + (PendingForceToApply * DeltaTime);

	// Don't call ClearAccumulatedForces() because it could affect launch velocity
	PendingImpulseToApply = FVector::ZeroVector;
	PendingForceToApply = FVector::ZeroVector;
}

bool UAircraftMovementComponent::HandlePendingLaunch()
{
	if (!PendingLaunchVelocity.IsZero() && HasValidData())
	{
		Velocity = PendingLaunchVelocity;
		// SetMovementMode(MOVE_Falling);
		PendingLaunchVelocity = FVector::ZeroVector;
		bForceNextFloorCheck = true;
		return true;
	}

	return false;
}

void UAircraftMovementComponent::ClearAccumulatedForces()
{
	PendingImpulseToApply = FVector::ZeroVector;
	PendingForceToApply = FVector::ZeroVector;
	PendingLaunchVelocity = FVector::ZeroVector;
}

void UAircraftMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (!(bOrientRotationToMovement || bUseControllerDesiredRotation))
	{
		return;
	}

	if (!HasValidData() /*|| (!VehicleOwner->Controller && !bRunPhysicsWithNoController)*/)
	{
		return;
	}

	FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
	CurrentRotation.DiagnosticCheckNaN(TEXT("VehicleMovementComponent::PhysicsRotation(): CurrentRotation"));

	FRotator DeltaRot = GetDeltaRotation(DeltaTime);
	DeltaRot.DiagnosticCheckNaN(TEXT("VehicleMovementComponent::PhysicsRotation(): GetDeltaRotation"));

	FRotator DesiredRotation = CurrentRotation;
	if (bOrientRotationToMovement)
	{
		DesiredRotation = ComputeOrientToMovementRotation(CurrentRotation, DeltaTime, DeltaRot);
	}
	else if (VehicleOwner->Controller && bUseControllerDesiredRotation)
	{
		DesiredRotation = VehicleOwner->Controller->GetDesiredRotation();
	}
	else if (!VehicleOwner->Controller /*&& bRunPhysicsWithNoController*/ && bUseControllerDesiredRotation)
	{
		if (AController* ControllerOwner = Cast<AController>(VehicleOwner->GetOwner()))
		{
			DesiredRotation = ControllerOwner->GetDesiredRotation();
		}
	}
	else
	{
		return;
	}

	if (ShouldRemainVertical())
	{
		DesiredRotation.Pitch = 0.f;
		DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
		DesiredRotation.Roll = 0.f;
	}
	else
	{
		DesiredRotation.Normalize();
	}

	// Accumulate a desired new rotation.
	const float AngleTolerance = 1e-3f;

	if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
	{
		// PITCH
		if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
		{
			DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
		}

		// YAW
		if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
		{
			DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
		}

		// ROLL
		if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
		{
			DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
		}

		// Set the new rotation.
		DesiredRotation.DiagnosticCheckNaN(TEXT("VehicleMovementComponent::PhysicsRotation(): DesiredRotation"));
		MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
	}
}

bool UAircraftMovementComponent::ShouldRemainVertical() const
{
	// Always remain vertical when landed.
	return IsMovingOnGround();
}

void UAircraftMovementComponent::StartNewPhysics(float deltaTime, int32 Iterations)
{
	if ((deltaTime < MIN_TICK_TIME) || (Iterations >= MaxSimulationIterations) || !HasValidData())
	{
		return;
	}

	if (UpdatedComponent->IsSimulatingPhysics())
	{
		// UE_LOG(LogVehicleMovement, Log, TEXT("UVehicleMovementComponent::StartNewPhysics: UpdateComponent (%s) is simulating physics - aborting."), *UpdatedComponent->GetPathName());
		return;
	}

	const bool bSavedMovementInProgress = bMovementInProgress;
	bMovementInProgress = true;

	switch (MovementMode)
	{
	case MOVE_None:
	case MOVE_Walking:
	case MOVE_NavWalking:
	case MOVE_Falling:
		break;
	case MOVE_Flying:
		PhysFlying(deltaTime, Iterations);
		break;
	case MOVE_Swimming:
	case MOVE_Custom:
	default:
		// UE_LOG(LogVehicleMovement, Warning, TEXT("%s has unsupported movement mode %d"), *VehicleOwner->GetName(), int32(MovementMode));
		// SetMovementMode(MOVE_None);
		break;
	}

	bMovementInProgress = bSavedMovementInProgress;
	/*if (bDeferUpdateMoveComponent)
	{
		SetUpdatedComponent(DeferredUpdatedMoveComponent);
	}*/
}

void UAircraftMovementComponent::PhysFlying(float DeltaTime, int32 Iterations)
{

}

float GetAxisDeltaRotation(float InAxisRotationRate, float DeltaTime)
{
	// Values over 360 don't do anything, see FMath::FixedTurn. However we are trying to avoid giant floats from overflowing other calculations.
	return (InAxisRotationRate >= 0.f) ? FMath::Min(InAxisRotationRate * DeltaTime, 360.f) : 360.f;
}

FRotator UAircraftMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	return FRotator(GetAxisDeltaRotation(RotationRate.Pitch, DeltaTime), GetAxisDeltaRotation(RotationRate.Yaw, DeltaTime), GetAxisDeltaRotation(RotationRate.Roll, DeltaTime));
}

FRotator UAircraftMovementComponent::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const
{
	if (Acceleration.SizeSquared() < UE_KINDA_SMALL_NUMBER)
	{
		// AI path following request can orient us in that direction (it's effectively an acceleration)
		/*if (bHasRequestedVelocity && RequestedVelocity.SizeSquared() > UE_KINDA_SMALL_NUMBER)
		{
			return RequestedVelocity.GetSafeNormal().Rotation();
		}*/

		// Don't change rotation if there is no acceleration.
		return CurrentRotation;
	}

	// Rotate toward direction of acceleration.
	return Acceleration.GetSafeNormal().Rotation();
}

FVector UAircraftMovementComponent::ConstrainInputAcceleration(const FVector& InputAcceleration) const
{
	// walking or falling pawns ignore up/down sliding
	if (InputAcceleration.Z != 0.f)
	{
		return FVector(InputAcceleration.X, InputAcceleration.Y, 0.f);
	}

	return InputAcceleration;
}

FVector UAircraftMovementComponent::ScaleInputAcceleration(const FVector& InputAcceleration) const
{
	return GetMaxAcceleration() * InputAcceleration.GetClampedToMaxSize(1.0f);
}

float UAircraftMovementComponent::GetMaxAcceleration() const
{
	return 1.0f;
}

void UAircraftMovementComponent::AddThrust(float Val)
{
	Thrust = FMath::Clamp<float>(Thrust + (Val * ThrustScale), 0.f, MaxThrust);
}



FString UAircraftMovementComponent::GetMovementName() const
{
	if (VehicleOwner)
	{
		if (VehicleOwner->GetRootComponent() && VehicleOwner->GetRootComponent()->IsSimulatingPhysics())
		{
			return TEXT("Rigid Body");
		}
	}

	// Using Vehicle movement
	/*switch (MovementMode)
	{
	case MOVE_None:				return TEXT("NULL"); break;
	case MOVE_Walking:			return TEXT("Walking"); break;
	case MOVE_NavWalking:		return TEXT("NavWalking"); break;
	case MOVE_Falling:			return TEXT("Falling"); break;
	case MOVE_Swimming:			return TEXT("Swimming"); break;
	case MOVE_Flying:			return TEXT("Flying"); break;
	case MOVE_Custom:			return TEXT("Custom"); break;
	default:					break;
	}*/
	return TEXT("Unknown");
}

TREASUREDCOVE_API FArchive& operator<<(FArchive& Ar, FVehicleReplaySample& V)
{
	// TODO: insert return statement here
	return Ar;
}

FNetworkPredictionData_Client_Vehicle::FNetworkPredictionData_Client_Vehicle(const UAircraftMovementComponent& ClientMovement)
{
}

FNetworkPredictionData_Client_Vehicle::~FNetworkPredictionData_Client_Vehicle()
{
}

void FNetworkPredictionData_Client_Vehicle::AddStructReferencedObjects(FReferenceCollector& Collector) const
{
}

int32 FNetworkPredictionData_Client_Vehicle::GetSavedMoveIndex(float TimeStamp) const
{
	return int32();
}

void FNetworkPredictionData_Client_Vehicle::AckMove(int32 AckedMoveIndex, UAircraftMovementComponent& AircraftMovementComponent)
{
}

//FSavedMovePtr FNetworkPredictionData_Client_Vehicle::AllocateNewMove()
//{
//	return FSavedMovePtr();
//}

//void FNetworkPredictionData_Client_Vehicle::FreeMove(const FSavedMovePtr& Move)
//{
//}

//FSavedMovePtr FNetworkPredictionData_Client_Vehicle::CreateSavedMove()
//{
//	return FSavedMovePtr();
//}

float FNetworkPredictionData_Client_Vehicle::UpdateTimeStampAndDeltaTime(float DeltaTime, AGAVehicle& VehicleOwner, UAircraftMovementComponent& AircraftMovementComponent)
{
	return 0.0f;
}

FNetworkPredictionData_Server_Vehicle::FNetworkPredictionData_Server_Vehicle(const UAircraftMovementComponent& ServerMovement)
{
}

FNetworkPredictionData_Server_Vehicle::~FNetworkPredictionData_Server_Vehicle()
{
}

float FNetworkPredictionData_Server_Vehicle::GetServerMoveDeltaTime(float ClientTimeStamp, float ActorTimeDilation) const
{
	return 0.0f;
}

float FNetworkPredictionData_Server_Vehicle::GetBaseServerMoveDeltaTime(float ClientTimeStamp, float ActorTimeDilation) const
{
	return 0.0f;
}
