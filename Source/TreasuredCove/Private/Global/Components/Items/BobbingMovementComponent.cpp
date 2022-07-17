// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Items/BobbingMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UBobbingMovementComponent::BeginPlay()
{
	if (!bInitialized)
	{
		InitialLocation = UpdatedComponent->GetComponentLocation();
		bInitialized = true;
	}

	Super::BeginPlay();
}

void UBobbingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bInitialized)
	{
		InitialLocation = UpdatedComponent->GetComponentLocation();
		bInitialized = true;
	}

	if (DeltaTime <= SMALL_NUMBER) return;

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const float MaxZ = InitialLocation.Z + MaxDeviation;
	const float MinZ = InitialLocation.Z - MaxDeviation;
	if (OldLocation.Z >= MaxZ || OldLocation.Z <= MinZ) Direction = 0 - Direction;

	FVector NewLocation = OldLocation;
	const float Deviation = Speed * Direction * DeltaTime;
	NewLocation.Z += Deviation;

	const FVector DeltaLocation = OldLocation - NewLocation;
	const bool bEnableCollision = false;
	MoveUpdatedComponent(DeltaLocation, UpdatedComponent->GetComponentRotation(), bEnableCollision);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
