// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementTrailComponent.h"
#include "MovementTrailActor.h"
#include "Particles/ParticleSystemComponent.h"
#include "GACharacter.h"

// Sets default values for this component's properties
UMovementTrailComponent::UMovementTrailComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	ComponentTags.Add( FName("GrassAffector") );
	ComponentTags.Add( FName("WaterAffector") );

	MaxActors = 50;
	MinMoveDistance = 50.0;
	LastActorLocation = FVector::ZeroVector;
}

void UMovementTrailComponent::SetSpawnActors(bool InShouldSpawn)
{
	bShouldSpawn = InShouldSpawn;

	if (!bShouldSpawn)
	{
		for (const auto& Actor : TrailActors)
		{
			Actor->Destroy();
		}

		TrailActors.Empty();
	}
}

void UMovementTrailComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMovementTrailComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DeltaTime < SMALL_NUMBER) return;

	if (bShouldSpawn && GetOwner() && (LastActorLocation - GetOwner()->GetActorLocation()).Size() > MinMoveDistance)
	{
		LastActorLocation = GetOwner()->GetActorLocation();

		if (TrailActors.Num() > MaxActors)
		{
			TrailActors.RemoveAt(0);
		}

		AMovementTrailActor* NewActor =
			NewObject<AMovementTrailActor>();

		TrailActors.Add(NewActor);
	}
}

