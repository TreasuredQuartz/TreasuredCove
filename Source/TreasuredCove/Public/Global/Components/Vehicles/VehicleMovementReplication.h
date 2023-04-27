// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementReplication.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "VehicleMovementReplication.generated.h"

class AGAVehicle;
class FSavedMove_Vehicle;
class UAircraftMovementComponent;

// Number of bits to reserve in serialization container. Make this large enough to try to avoid re-allocation during the worst case RPC calls (dual move + unacknowledged "old important" move).
#ifndef VEHICLE_SERIALIZATION_PACKEDBITS_RESERVED_SIZE
#define VEHICLE_SERIALIZATION_PACKEDBITS_RESERVED_SIZE 1024
#endif

//////////////////////////////////////////////////////////////////////////
/**
 * Intermediate data stream used for network serialization of Character RPC data.
 * This is basically an array of bits that is packed/unpacked via NetSerialize into custom data structs on the sending and receiving ends.
 */
USTRUCT()
struct TREASUREDCOVE_API FVehicleNetworkSerializationPackedBits
{
	GENERATED_USTRUCT_BODY()

		FVehicleNetworkSerializationPackedBits()
		: SavedPackageMap(nullptr)
	{
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess);
	UPackageMap* GetPackageMap() const { return SavedPackageMap; }

	//------------------------------------------------------------------------
	// Data

	// TInlineAllocator used with TBitArray takes the number of 32-bit dwords, but the define is in number of bits, so convert here by dividing by 32.
	TBitArray<TInlineAllocator<VEHICLE_SERIALIZATION_PACKEDBITS_RESERVED_SIZE / NumBitsPerDWORD>> DataBits;

#if UE_WITH_IRIS
	// Since this struct uses custom serialization path we need to explicitly capture object references, this is managed by the use of a custom packagemap.
	UIrisObjectReferencePackageMap::FObjectReferenceArray ObjectReferences;
#endif

private:
	UPackageMap* SavedPackageMap;
};

template<>
struct TStructOpsTypeTraits<FVehicleNetworkSerializationPackedBits> : public TStructOpsTypeTraitsBase2<FVehicleNetworkSerializationPackedBits>
{
	enum
	{
		WithNetSerializer = true,
	};
};

//////////////////////////////////////////////////////////////////////////
// Server to Client response
//////////////////////////////////////////////////////////////////////////

struct TREASUREDCOVE_API FVehicleNetworkMoveData
{
public:

	enum class ENetworkMoveType
	{
		NewMove,
		PendingMove,
		OldMove
	};

	FVehicleNetworkMoveData()
		: NetworkMoveType(ENetworkMoveType::NewMove)
		, TimeStamp(0.f)
		, Acceleration(ForceInitToZero)
		, Location(ForceInitToZero)
		, ControlRotation(ForceInitToZero)
		, CompressedMoveFlags(0)
		, MovementMode(0)
		, MovementBase(nullptr)
		, MovementBaseBoneName(NAME_None)
	{
	};

	virtual ~FVehicleNetworkMoveData()
	{
	};

	/**
	 * Given a FSavedMove_Character from UCharacterMovementComponent, fill in data in this struct with relevant movement data.
	 * Note that the instance of the FSavedMove_Character is likely a custom struct of a derived struct of your own, if you have added your own saved move data.
	 * @see UCharacterMovementComponent::AllocateNewMove()
	 */
	virtual void ClientFillNetworkMoveData(const FSavedMove_Vehicle& ClientMove, ENetworkMoveType MoveType);

	/**
	 * Serialize the data in this struct to or from the given FArchive. This packs or unpacks the data in to a variable-sized data stream that is sent over the
	 * network from client to server.
	 * @see UCharacterMovementComponent::CallServerMovePacked
	 */
	virtual bool Serialize(UAircraftMovementComponent& AircraftMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType);

	// Indicates whether this was the latest new move, a pending/dual move, or old important move.
	ENetworkMoveType NetworkMoveType;

	//------------------------------------------------------------------------
	// Basic movement data.

	float TimeStamp;
	FVector_NetQuantize10 Acceleration;
	FVector_NetQuantize100 Location;		// Either world location or relative to MovementBase if that is set.
	FRotator ControlRotation;
	uint8 CompressedMoveFlags;

	uint8 MovementMode;
	class UPrimitiveComponent* MovementBase;
	FName MovementBaseBoneName;
};

//////////////////////////////////////////////////////////////////////////
/**
 * Struct used for network RPC parameters between client/server by ACharacter and UCharacterMovementComponent.
 * To extend network move data and add custom parameters, you typically override this struct with a custom derived struct and set the CharacterMovementComponent
 * to use your container with UCharacterMovementComponent::SetNetworkMoveDataContainer(). Your derived struct would then typically (in the constructor) replace the
 * NewMoveData, PendingMoveData, and OldMoveData pointers to use your own instances of a struct derived from FCharacterNetworkMoveData, where you add custom fields
 * and implement custom serialization to be able to pack and unpack your own additional data.
 *
 * @see UCharacterMovementComponent::SetNetworkMoveDataContainer()
 */
struct TREASUREDCOVE_API FVehicleNetworkMoveDataContainer
{
public:

	/**
	 * Default constructor. Sets data storage (NewMoveData, PendingMoveData, OldMoveData) to point to default data members. Override those pointers to instead point to custom data if you want to use derived classes.
	 */
	FVehicleNetworkMoveDataContainer()
		: bHasPendingMove(false)
		, bIsDualHybridRootMotionMove(false)
		, bHasOldMove(false)
		, bDisableCombinedScopedMove(false)
	{
		NewMoveData = &BaseDefaultMoveData[0];
		PendingMoveData = &BaseDefaultMoveData[1];
		OldMoveData = &BaseDefaultMoveData[2];
	};

	virtual ~FVehicleNetworkMoveDataContainer()
	{
	};

	/**
	 * Passes through calls to ClientFillNetworkMoveData on each FCharacterNetworkMoveData matching the client moves. Note that ClientNewMove will never be null, but others may be.
	 */
	virtual void ClientFillNetworkMoveData(const FSavedMove_Vehicle* ClientNewMove, const FSavedMove_Vehicle* ClientPendingMove, const FSavedMove_Vehicle* ClientOldMove);

	/**
	 * Serialize movement data. Passes Serialize calls to each FCharacterNetworkMoveData as applicable, based on bHasPendingMove and bHasOldMove.
	 */
	virtual bool Serialize(UAircraftMovementComponent& VehicleMovement, FArchive& Ar, UPackageMap* PackageMap);

	//------------------------------------------------------------------------
	// Basic movement data. NewMoveData is the most recent move, PendingMoveData is a move right before it (dual move). OldMoveData is an "important" move not yet acknowledged.

	FORCEINLINE FVehicleNetworkMoveData* GetNewMoveData() const { return NewMoveData; }
	FORCEINLINE FVehicleNetworkMoveData* GetPendingMoveData() const { return PendingMoveData; }
	FORCEINLINE FVehicleNetworkMoveData* GetOldMoveData() const { return OldMoveData; }

	//------------------------------------------------------------------------
	// Optional pending data used in "dual moves".
	bool bHasPendingMove;
	bool bIsDualHybridRootMotionMove;

	// Optional "old move" data, for redundant important old moves not yet ack'd.
	bool bHasOldMove;

	// True if we want to disable a scoped move around both dual moves (optional from bEnableServerDualMoveScopedMovementUpdates), typically set if bForceNoCombine was true which can indicate an important change in moves.
	bool bDisableCombinedScopedMove;

protected:

	FVehicleNetworkMoveData* NewMoveData;
	FVehicleNetworkMoveData* PendingMoveData;	// Only valid if bHasPendingMove is true
	FVehicleNetworkMoveData* OldMoveData;		// Only valid if bHasOldMove is true

private:

	FVehicleNetworkMoveData BaseDefaultMoveData[3];
};

//////////////////////////////////////////////////////////////////////////
/**
 * Response from the server to the client about a move that is being acknowledged.
 * Internally it mainly copies the FClientAdjustment from the UCharacterMovementComponent indicating the response, as well as
 * setting a few relevant flags about the response and serializing the response to and from an FArchive for handling the variable-size
 * payload over the network.
 */
struct TREASUREDCOVE_API FVehicleMoveResponseDataContainer
{
public:

	FVehicleMoveResponseDataContainer()
		: bHasBase(false)
		, bHasRotation(false)
	{
	}

	virtual ~FVehicleMoveResponseDataContainer()
	{
	}

	/**
	 * Copy the FClientAdjustment and set a few flags relevant to that data.
	 */
	virtual void ServerFillResponseData(const UAircraftMovementComponent& VehicleMovement, const FClientAdjustment& PendingAdjustment);

	/**
	 * Serialize the FClientAdjustment data and other internal flags.
	 */
	virtual bool Serialize(UAircraftMovementComponent& VehicleMovement, FArchive& Ar, UPackageMap* PackageMap);

	bool IsGoodMove() const { return ClientAdjustment.bAckGoodMove; }
	bool IsCorrection() const { return !IsGoodMove(); }

	bool bHasBase;
	bool bHasRotation; // By default ClientAdjustment.NewRot is not serialized. Set this to true after base ServerFillResponseData if you want Rotation to be serialized.

	// Client adjustment. All data other than bAckGoodMove and TimeStamp is only valid if this is a correction (not an ack).
	FClientAdjustment ClientAdjustment;

};

//////////////////////////////////////////////////////////////////////////
/**
 * Structure used internally to handle serialization of FCharacterNetworkMoveDataContainer over the network.
 */
USTRUCT()
struct TREASUREDCOVE_API FVehicleServerMovePackedBits : public FVehicleNetworkSerializationPackedBits
{
	GENERATED_USTRUCT_BODY()

public:
	FVehicleServerMovePackedBits() {}
};

template<>
struct TStructOpsTypeTraits<FVehicleServerMovePackedBits> : public TStructOpsTypeTraitsBase2<FVehicleServerMovePackedBits>
{
	enum
	{
		WithNetSerializer = true,
	};
};

