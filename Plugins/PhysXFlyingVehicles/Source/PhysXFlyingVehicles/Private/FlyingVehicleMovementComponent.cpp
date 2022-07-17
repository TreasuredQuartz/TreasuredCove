// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingVehicleMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

UFlyingVehicleMovementComponent::UFlyingVehicleMovementComponent()
{
	Mass = 100;
	Velocity = FVector(0);
	Acceleration = FVector(100.0f);
}

void UFlyingVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

