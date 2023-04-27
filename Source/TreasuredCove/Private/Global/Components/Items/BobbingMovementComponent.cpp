// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Items/BobbingMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UBobbingMovementComponent::BeginPlay()
{
	if (!bInitialized && UpdatedComponent)
	{
		InitialLocation = UpdatedComponent->GetComponentLocation();
		bInitialized = true;
	}

	Super::BeginPlay();
}

void UBobbingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// skip if we don't want component updated when not rendered or if updated component can't move
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}
	if (!IsValid(UpdatedComponent))
	{
		return;
	}
	if (!bInitialized)
	{
		InitialLocation = UpdatedComponent->GetComponentLocation();
		bInitialized = true;
	}

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const float MaxZ = InitialLocation.Z + (2 * MaxDeviation);
	const float MinZ = InitialLocation.Z;
	if (OldLocation.Z >= MaxZ || OldLocation.Z <= MinZ) Direction = 0 - Direction;


	FVector Delta = FVector::ZeroVector;
	const float Deviation = Speed * Direction * DeltaTime;
	Delta.Z += Deviation;

	const bool bEnableCollision = false;
	MoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), bEnableCollision);
}
