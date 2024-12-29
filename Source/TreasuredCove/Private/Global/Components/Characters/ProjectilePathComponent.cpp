// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/ProjectilePathComponent.h"
#include "Global/Actors/ProjectilePathActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UProjectilePathComponent::UProjectilePathComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UProjectilePathComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
//void UProjectilePathComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UProjectilePathComponent::BeginPrediction_Client_Implementation(const FVector& StartLocation, const FVector& LaunchVelocity)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(13, 5.0, FColor::Red, "Begin Prediction!");

	// We need the class specified before we can spawn text actors!
	if (!PathActorClass || PathActor)
		return;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(13, 5.0, FColor::Red, "Path Actor Class is Valid!");

	const FTransform spawnTransform = FTransform(StartLocation);
	AProjectilePathActor* newPathActor = GetWorld()->SpawnActorDeferred<AProjectilePathActor>(PathActorClass, spawnTransform, GetOwner(), GetOwner<APawn>(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// Possible to get bad spawns and this checks that we did in fact spawn
	if (!newPathActor)
		return;
	
	newPathActor->OnDestroyed.AddDynamic(this, &UProjectilePathComponent::OnPathDestroyed);
	newPathActor->SetOwner(GetOwner());
	newPathActor->SetLaunchVelocity(LaunchVelocity);
	newPathActor->SetProjectileCollisionRadius(1.f);
	UGameplayStatics::FinishSpawningActor(newPathActor, spawnTransform);

	PathActor = newPathActor;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(14, 5.0, FColor::Red, "Spawning Actor!");
}

void UProjectilePathComponent::StopPrediction()
{
	if (PathActor)
	{
		PathActor->Destroy();
		PathActor = nullptr;
	}
}

void UProjectilePathComponent::UpdatePrediction(const FVector& InStartLocation, const FVector& LaunchVelocity)
{
	if (!PathActor) return;

	PathActor->SetActorLocation(InStartLocation);
	PathActor->SetLaunchVelocity(LaunchVelocity);
}

void UProjectilePathComponent::OnPathDestroyed(AActor* DestroyedActor)
{
	PathActor = nullptr;
}

