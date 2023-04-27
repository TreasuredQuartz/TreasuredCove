// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Vehicles/VehicleMovementReplication.h"

bool FVehicleNetworkSerializationPackedBits::NetSerialize(FArchive& Ar, UPackageMap* PackageMap, bool& bOutSuccess)
{
	return false;
}

void FVehicleNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Vehicle& ClientMove, ENetworkMoveType MoveType)
{
}

bool FVehicleNetworkMoveData::Serialize(UAircraftMovementComponent& AircraftMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	return false;
}

void FVehicleNetworkMoveDataContainer::ClientFillNetworkMoveData(const FSavedMove_Vehicle* ClientNewMove, const FSavedMove_Vehicle* ClientPendingMove, const FSavedMove_Vehicle* ClientOldMove)
{
}

bool FVehicleNetworkMoveDataContainer::Serialize(UAircraftMovementComponent& VehicleMovement, FArchive& Ar, UPackageMap* PackageMap)
{
	return false;
}

void FVehicleMoveResponseDataContainer::ServerFillResponseData(const UAircraftMovementComponent& VehicleMovement, const FClientAdjustment& PendingAdjustment)
{
}

bool FVehicleMoveResponseDataContainer::Serialize(UAircraftMovementComponent& VehicleMovement, FArchive& Ar, UPackageMap* PackageMap)
{
	return false;
}
