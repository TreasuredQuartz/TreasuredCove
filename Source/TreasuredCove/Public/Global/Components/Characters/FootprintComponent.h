// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootprintComponent.generated.h"

class UCharacterMovementComponent;
class AFootprintActor;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UFootprintComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Footprint", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AFootprintActor> FootprintActorClass = nullptr;

	// Footstep Locations for each foot in order of spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footprint", Meta = (AllowPrivateAccess = "true"))
	TArray<FVector> FootprintLocations;

	uint8 CurrentFootstep;
	float DistanceWalked;
	UCharacterMovementComponent* OwnerCharacterMovement;

	// Distance in between footsteps
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Footprint", Meta = (AllowPrivateAccess = "true"))
	float FootstepDistance;

public:
	// Sets default values for this component's properties
	UFootprintComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Adds specified text at specified location
	UFUNCTION(BlueprintCallable, Category = "FloatingText")
	void AddFootprint(const FVector& WorldLocation, int PhysSurfaceType);

	// Callable function to destroy all currently active text actors (On Death for instance)
	UFUNCTION()
	void DestroyAllFootprints();

	// Checks if a footprint should be placed
	void FootprintCheck();

protected:
	// Callback function called when a floating text actior is destroyed
	UFUNCTION()
	void OnFootprintDestroyed(AActor* DestroyedActor);

	// An array of all active floating text actors
	UPROPERTY()
	TArray<AFootprintActor*> ActiveFootprintActors;
};
