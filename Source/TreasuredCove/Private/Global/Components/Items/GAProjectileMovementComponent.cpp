// Fill out your copyright notice in the Description page of Project Settings.

#include "GAProjectileMovementComponent.h"
#include "Gameframework/PhysicsVolume.h"

void UGAProjectileMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	CalculateArea();
}

void UGAProjectileMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	const FVector& Start = UpdatedComponent->GetComponentLocation(); // Location before move?

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // Moves us?

	FHitResult Hit;
	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(Hit, Start, Start + Velocity, ECollisionChannel::ECC_Pawn)) // Velocity after move?
	{
		OnProjectileHitPawn.Broadcast(Hit);
	}
}

FVector UGAProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	FVector Ret = Super::ComputeAcceleration(InVelocity, DeltaTime);

	if (bEnablePivotAxis)
	{
		// Our location before moving this frame
		FVector MyLocation = UpdatedComponent->GetComponentLocation();

		// Parallel Line with our current direction
		FVector P = FMath::ClosestPointOnInfiniteLine(PivotAxisStart, PivotAxisStart + PivotAxisDirection * 5000.f, MyLocation);

		FVector CurrentOffset = MyLocation - P;
		float CurrentLength = CurrentOffset.Size();
		CurrentOffset.Normalize();

		// The direction we need to go in
		FVector Tangent = FVector::CrossProduct(PivotAxisDirection, CurrentOffset);

		// Move Back toward center if we drift
		FVector DriftCorrection = (PivotAxisOffset - CurrentLength) * CurrentOffset;

		Ret += (Tangent.GetSafeNormal() + DriftCorrection) * PivotAxisRotationRate;
	}

	if (bEnableDrag)
	{
		Ret -= ComputeDrag(InVelocity);
	}

	return Ret;
}

// D = Cd * (r * V^2) * A
FVector UGAProjectileMovementComponent::ComputeDrag(const FVector& InVelocity) const
{
	APhysicsVolume* PhysicsVolume = GetPhysicsVolume();
	// kg/m^3
	float AirDensity = PhysicsVolume && PhysicsVolume->bWaterVolume ? PhysicsVolume->FluidFriction : 1.225;

	return DragCoefficient * ((AirDensity * InVelocity * InVelocity) * 0.5) * Area;
}

void UGAProjectileMovementComponent::CalculateArea()
{
	Area = Diameter * 0.5 * PI;
}

float CalculateSurfaceArea()
{
	TArray<FVector> Vertices, Normals;
	TArray<int32> Triangles;
	TArray<FVector2D> UVs;
	// TArray<FProcTangent> Tangents;

	double Sum = 0.0;
	for (int32 Index = 0; Index < Triangles.Num(); Index += 3)
	{
		FVector Corner = Vertices[Triangles[Index]];
		FVector A = Vertices[Triangles[Index + 1]];
		FVector B = Vertices[Triangles[Index + 2]];

		Sum += FVector::CrossProduct(A, B).Size(); // Triangle height
	}

	return (float)(Sum * 0.5); // Triangle Area = Height / 2
}
float CalculateFacingArea(const FVector& Direction)
{
	TArray<FVector> Vertices, Normals;
	TArray<int32> Triangles;
	TArray<FVector2D> UVs;
	// TArray<FProcTangent> Tangents;

	double Sum = 0.0;
	for (int32 Index = 0; Index < Triangles.Num(); Index += 3)
	{
		FVector Corner = Vertices[Triangles[Index]];
		FVector A = Vertices[Triangles[Index + 1]];
		FVector B = Vertices[Triangles[Index + 2]];

		float Projection = FVector::DotProduct(FVector::CrossProduct(A, B), Direction);

		Sum += Projection > 0 ? Projection : 0;
	}

	return (float)(Sum * 0.5); // Triangle Area = Height / 2
}

