// Fill out your copyright notice in the Description page of Project Settings.

#include "GAProjectileMovementComponent.h"

FVector UGAProjectileMovementComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	FVector Ret = Super::ComputeAcceleration(InVelocity, DeltaTime);

	if (!bEnablePivotAxis)
	{
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
	return Ret;
}