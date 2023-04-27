// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementTrailComponent.generated.h"

class AMovementTrailActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UMovementTrailComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	bool bShouldSpawn;

	int MaxActors;

	// the distance between brush spawns
	float MinMoveDistance;

	// The last location that we spawned
	FVector LastActorLocation;

	// The trail actor to spawn at runtime.
	TSubclassOf<AMovementTrailActor> TrailClasses;

	// Referrences to currently spawned actors
	TArray<AMovementTrailActor*> TrailActors;

public:
	// Sets default values for this actor's properties
	UMovementTrailComponent();

	void SetSpawnActors(bool InShouldSpawn);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
