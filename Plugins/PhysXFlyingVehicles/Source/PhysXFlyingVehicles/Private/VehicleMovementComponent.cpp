


#include "VehicleMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

UVehicleMovementComponent::UVehicleMovementComponent()
{

}

void UVehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	/* Multiply all calculations dependent on time by Delta time. 
	* This creates frame rate independent speed.
	*/

	// Check to make sure we are valid to move this frame before actual moving
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		UpdateForwardForce(DeltaTime);
		UpdateDownwardForce(DeltaTime);
		UpdateUpwardForce(DeltaTime);
		UpdateBackwardForce(DeltaTime);

		SolveForNetForce();
		if (NetForce.Size() != 0)
		{
			Velocity += NetForce;
			FHitResult Hit;
			SafeMoveUpdatedComponent(Velocity, UpdatedComponent->GetComponentRotation(), true, Hit);
		}
	}
}

void UVehicleMovementComponent::Update(float DeltaTime)
{
	Super::Update(DeltaTime);
}

void UVehicleMovementComponent::UpdateState(float DeltaTime)
{
	Super::UpdateState(DeltaTime);
}

void UVehicleMovementComponent::UpdateForwardForce(float DeltaTime)
{
	// Gets (and clears) the current frame input vector
	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f);

	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		// This will not actually move us at this point.
		Thrust += DesiredMovementThisFrame * DeltaTime * Acceleration;
	}
}

void UVehicleMovementComponent::UpdateBackwardForce(float DeltaTime)
{
	// SkinFriction = OurRoughness * FluidViscosity;
	// FormDrag = LocalPressure * TotalSurfaceArea
	// if(Lift.Size() != 0) InducedDrag = pow(LiftCoefficient, 2) / (3.14159 * AspectRatio * Efficiency)
	// WaveDrag = (Mach = FlowVelocity / SpeedOfSound)
	// RamDrag = CoolingInlets + JetEngines
	// DragCoefficient = SkinFriction + FormDrag + WaveDrag + RamDrag + InducedDrag
	// Drag = DragCoefficient * Area * 0.5 * Density * pow(Velocity, 2)

	if (Velocity.Size() != 0)
	{

	}
}

void UVehicleMovementComponent::UpdateUpwardForce(float DeltaTime)
{
	// Airspeed = Groundspeed - Windspeed
	// AirVelocity
	// Wing Incline
	// FluidViscosity FluidCompressibility
	// AspectRatio = Span / ChordLength // Higher Ratio = Less Drag and More Lift
	// Lift = LiftCoefficient * Area * 0.5 * AirDensity * pow(Velocity, 2)

	if (Velocity.Size() != 0)
	{

	}
}

void UVehicleMovementComponent::UpdateDownwardForce(float DeltaTime)
{
	Weight = Gravity * Mass;
}

void UVehicleMovementComponent::SolveForNetForce()
{
	NetForce = Thrust + Lift + Weight + Drag;
}
