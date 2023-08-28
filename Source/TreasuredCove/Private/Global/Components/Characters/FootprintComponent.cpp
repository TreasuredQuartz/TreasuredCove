// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/FootprintComponent.h"
#include "Global/Actors/FootprintActor.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UFootprintComponent::UFootprintComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UFootprintComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFootprintComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (DeltaTime < SMALL_NUMBER) return;

	if (!GetOwner() || FootprintLocations.IsEmpty()) return;

	FootprintCheck();
}

void UFootprintComponent::AddFootprint(const FVector& WorldLocation, int PhysSurfaceType)
{
	if (!FootprintActorClass)
		return;

	FTransform SpawnTransform = FTransform(GetOwner()->GetActorRotation(), WorldLocation, FVector(1,1,1));
	AFootprintActor* NewFootprint = GetWorld()->SpawnActorDeferred<AFootprintActor>(FootprintActorClass, SpawnTransform, GetOwner(), GetOwner<APawn>(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!NewFootprint)
		return;

	NewFootprint->OnDestroyed.AddDynamic(this, &UFootprintComponent::OnFootprintDestroyed);
	NewFootprint->SetSurfaceType(PhysSurfaceType);
	NewFootprint->SetFootprintType(CurrentFootstep);
	NewFootprint->SetOwner(GetOwner());

	UGameplayStatics::FinishSpawningActor(NewFootprint, SpawnTransform);

	ActiveFootprintActors.Insert(NewFootprint, 0); 
	++CurrentFootstep;
}

void UFootprintComponent::DestroyAllFootprints()
{
	ActiveFootprintActors.Empty();
}

void UFootprintComponent::OnFootprintDestroyed(AActor* DestroyedActor)
{
	ActiveFootprintActors.Pop();
}

void UFootprintComponent::FootprintCheck()
{
	// UE_LOG(LogTemp, Warning, TEXT("[Footprint Component] Footprint Check"));

	if (OwnerCharacterMovement)
	{
		if (OwnerCharacterMovement->IsWalking() && !OwnerCharacterMovement->IsCrouching())
		{
			DistanceWalked += GetOwner()->GetVelocity().Length() / 100;

			if (DistanceWalked > FootstepDistance)
			{
				DistanceWalked = 0;

				if (CurrentFootstep >= FootprintLocations.Num())
					CurrentFootstep = 0;

				AddFootprint(GetOwner()->GetActorLocation() + FootprintLocations[CurrentFootstep], OwnerCharacterMovement->CurrentFloor.bBlockingHit ? UGameplayStatics::GetSurfaceType(OwnerCharacterMovement->CurrentFloor.HitResult) : EPhysicalSurface::SurfaceType1);
			}
		}
		else
		{
			DistanceWalked = 0;
		}
	}
	else if (GetOwner<ACharacter>() && GetOwner<ACharacter>()->GetCharacterMovement())
	{
		OwnerCharacterMovement = GetOwner<ACharacter>()->GetCharacterMovement();
	}
	else if (UActorComponent* OwnerMovementComponent = GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()))
	{
		OwnerCharacterMovement = Cast<UCharacterMovementComponent>(OwnerMovementComponent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FootprintComponent: Owner does not have Character Movement Component."));
	}
}