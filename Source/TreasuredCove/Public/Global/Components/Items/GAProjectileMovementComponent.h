// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAProjectileMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGAProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectileHitPawnDelegate, const FHitResult&, ImpactResult);
	
protected:
	// Begin UObject Interface//
	virtual void BeginPlay() override;
	// End UObject Interface //
	
	// Begin UProjectileMovementComponent Interface //
	/** Compute the acceleration that will be applied */
	virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const override;
	// virtual FVector ComputeVelocity(FVector InitialVelocity, float DeltaTime) const override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End UProjectileMovementComponent Interface //

	FVector ComputeDrag(const FVector& InVelocity) const;
	void CalculateArea();
public:
	// Begin Bullet Drag Properties //
	/** Used for simplyfing drag equation per projectile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
	float DragCoefficient = 0.01;

	/** (used for drag, and could be used for applying force on bounce) */
	float GetMass() const { return ProjectileGravityScale; };

	/** Area calculated from diameter(Used for drag) */
	float Area;

	/** (Assuming 2D circle facing velocity) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
	float Diameter = 1.0;

	/** Enables Air friction. Intended for bullets. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag")
	bool bEnableDrag;
	// End Bullet Drag Properties //

public:
	// Begin Cool Spiral Effect //
	/** Acceleration applied for a set time after spawn */
	float PivotAxisRotationRate;

	/** The Location to apply acceleration */
	FVector PivotAxisStart;

	/** The Location's direction to apply acceleration */
	FVector PivotAxisDirection;

	/** The Location to apply directional acceleration's offset */
	FVector PivotAxisOffset;

	bool bEnablePivotAxis;
	// End Cool Spiral Effect //

public:
	UPROPERTY(BlueprintAssignable)
	FOnProjectileHitPawnDelegate OnProjectileHitPawn;
};
